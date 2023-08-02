#include "customchar/audio/sdl.h"

using namespace CC::audio;

AudioAsync::AudioAsync(int len_ms) {
  m_len_ms_ = len_ms;
  m_running_ = false;
}

AudioAsync::~AudioAsync() {
  if (m_dev_id_in_) {
    SDL_CloseAudioDevice(m_dev_id_in_);
  }
}

bool AudioAsync::initialize(int capture_id, int sample_rate) {
  SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO);

  if (SDL_Init(SDL_INIT_AUDIO) < 0) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s\n",
                 SDL_GetError());
    return false;
  }

  SDL_SetHintWithPriority(SDL_HINT_AUDIO_RESAMPLING_MODE, "medium",
                          SDL_HINT_OVERRIDE);

  {
    int nDevices = SDL_GetNumAudioDevices(SDL_TRUE);
    fprintf(stderr, "%s: found %d capture devices:\n", __func__, nDevices);
    for (int i = 0; i < nDevices; i++) {
      fprintf(stderr, "%s:    - Capture device #%d: '%s'\n", __func__, i,
              SDL_GetAudioDeviceName(i, SDL_TRUE));
    }
  }

  SDL_AudioSpec capture_spec_requested;
  SDL_AudioSpec capture_spec_obtained;

  SDL_zero(capture_spec_requested);
  SDL_zero(capture_spec_obtained);

  capture_spec_requested.freq = sample_rate;
  capture_spec_requested.format = AUDIO_F32;
  capture_spec_requested.channels = 1;
  capture_spec_requested.samples = 1024;
  capture_spec_requested.callback = [](void* userdata, uint8_t* stream,
                                       int len) {
    AudioAsync* audio = (AudioAsync*)userdata;
    audio->callback(stream, len);
  };
  capture_spec_requested.userdata = this;

  if (capture_id >= 0) {
    fprintf(stderr, "%s: attempt to open capture device %d : '%s' ...\n",
            __func__, capture_id, SDL_GetAudioDeviceName(capture_id, SDL_TRUE));
    m_dev_id_in_ = SDL_OpenAudioDevice(
        SDL_GetAudioDeviceName(capture_id, SDL_TRUE), SDL_TRUE,
        &capture_spec_requested, &capture_spec_obtained, 0);
  } else {
    fprintf(stderr, "%s: attempt to open default capture device ...\n",
            __func__);
    m_dev_id_in_ = SDL_OpenAudioDevice(
        nullptr, SDL_TRUE, &capture_spec_requested, &capture_spec_obtained, 0);
  }

  if (!m_dev_id_in_) {
    fprintf(stderr, "%s: couldn't open an audio device for capture: %s!\n",
            __func__, SDL_GetError());
    m_dev_id_in_ = 0;

    return false;
  } else {
    fprintf(stderr, "%s: obtained spec for input device (SDL Id = %d):\n",
            __func__, m_dev_id_in_);
    fprintf(stderr, "%s:     - sample rate:       %d\n", __func__,
            capture_spec_obtained.freq);
    fprintf(stderr, "%s:     - format:            %d (required: %d)\n",
            __func__, capture_spec_obtained.format,
            capture_spec_requested.format);
    fprintf(stderr, "%s:     - channels:          %d (required: %d)\n",
            __func__, capture_spec_obtained.channels,
            capture_spec_requested.channels);
    fprintf(stderr, "%s:     - samples per frame: %d\n", __func__,
            capture_spec_obtained.samples);
  }

  m_sample_rate_ = capture_spec_obtained.freq;
  m_audio_.resize((m_sample_rate_ * m_len_ms_) / 1000);
  return true;
}

bool AudioAsync::resume() {
  if (!m_dev_id_in_) {
    fprintf(stderr, "%s: no audio device to resume!\n", __func__);
    return false;
  }

  if (m_running_) {
    fprintf(stderr, "%s: already running!\n", __func__);
    return false;
  }

  SDL_PauseAudioDevice(m_dev_id_in_, 0);
  m_running_ = true;
  return true;
}

bool AudioAsync::pause() {
  if (!m_dev_id_in_) {
    fprintf(stderr, "%s: no audio device to pause!\n", __func__);
    return false;
  }

  if (!m_running_) {
    fprintf(stderr, "%s: already paused!\n", __func__);
    return false;
  }

  SDL_PauseAudioDevice(m_dev_id_in_, 1);

  m_running_ = false;
  return true;
}

bool AudioAsync::clear() {
  if (!m_dev_id_in_) {
    fprintf(stderr, "%s: no audio device to clear!\n", __func__);
    return false;
  }

  if (!m_running_) {
    fprintf(stderr, "%s: not running!\n", __func__);
    return false;
  }

  {
    std::lock_guard<std::mutex> lock(m_mutex_);

    m_audio_pos_ = 0;
    m_audio_len_ = 0;
  }

  return true;
}

void AudioAsync::callback(uint8_t* stream, int len) {
  if (!m_running_) {
    return;
  }

  const size_t n_samples = len / sizeof(float);

  m_audio_new_.resize(n_samples);
  memcpy(m_audio_new_.data(), stream, n_samples * sizeof(float));

  // fprintf(stderr, "%s: %zu samples, pos %zu, len %zu\n", __func__, n_samples,
  // m_audio_pos, m_audio_len);

  {
    std::lock_guard<std::mutex> lock(m_mutex_);

    if (m_audio_pos_ + n_samples > m_audio_.size()) {
      const size_t n0 = m_audio_.size() - m_audio_pos_;

      memcpy(&m_audio_[m_audio_pos_], stream, n0 * sizeof(float));
      memcpy(&m_audio_[0], &stream[n0], (n_samples - n0) * sizeof(float));

      m_audio_pos_ = (m_audio_pos_ + n_samples) % m_audio_.size();
      m_audio_len_ = m_audio_.size();
    } else {
      memcpy(&m_audio_[m_audio_pos_], stream, n_samples * sizeof(float));

      m_audio_pos_ = (m_audio_pos_ + n_samples) % m_audio_.size();
      m_audio_len_ = std::min(m_audio_len_ + n_samples, m_audio_.size());
    }
  }
}

void AudioAsync::get(int ms, std::vector<float>& result) {
  if (!m_dev_id_in_) {
    fprintf(stderr, "%s: no audio device to get audio from!\n", __func__);
    return;
  }

  if (!m_running_) {
    fprintf(stderr, "%s: not running!\n", __func__);
    return;
  }

  result.clear();

  {
    std::lock_guard<std::mutex> lock(m_mutex_);

    if (ms <= 0) {
      ms = m_len_ms_;
    }

    size_t n_samples = (m_sample_rate_ * ms) / 1000;
    if (n_samples > m_audio_len_) {
      n_samples = m_audio_len_;
    }

    result.resize(n_samples);

    int s0 = m_audio_pos_ - n_samples;
    if (s0 < 0) {
      s0 += m_audio_.size();
    }

    if (s0 + n_samples > m_audio_.size()) {
      const size_t n0 = m_audio_.size() - s0;

      memcpy(result.data(), &m_audio_[s0], n0 * sizeof(float));
      memcpy(&result[n0], &m_audio_[0], (n_samples - n0) * sizeof(float));
    } else {
      memcpy(result.data(), &m_audio_[s0], n_samples * sizeof(float));
    }
  }
}

bool CC::audio::sdl_poll_events() {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_QUIT: {
        return false;
      } break;
      default:
        break;
    }
  }

  return true;
}
