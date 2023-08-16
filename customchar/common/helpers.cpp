#include "customchar/common/helpers.h"

using namespace CC;
using namespace CC::common;

void common::cc_print_params_usage(int /*argc*/, char** argv,
                                   const CCParams& params) {
  fprintf(stderr, "\n");
  fprintf(stderr, "usage: %s [options]\n", argv[0]);
  fprintf(stderr, "\n");
  fprintf(stderr, "options:\n");
  fprintf(stderr,
          "  -h,       --help          [default] show this help message and "
          "exit\n");
  fprintf(stderr,
          "  -t N,     --threads N     [%-7d] number of threads to use during "
          "computation\n",
          params.n_threads);
  fprintf(stderr,
          "  -vms N,   --voice-ms N    [%-7d] voice duration in milliseconds\n",
          params.voice_ms);
  fprintf(stderr, "  -c ID,    --capture ID    [%-7d] capture device ID\n",
          params.capture_id);
  fprintf(stderr,
          "  -mt N,    --max-tokens N  [%-7d] maximum number of tokens per "
          "audio chunk\n",
          params.max_tokens);
  fprintf(stderr,
          "  -ac N,    --audio-ctx N   [%-7d] audio context size (0 - all)\n",
          params.audio_ctx);
  fprintf(stderr,
          "  -vth N,   --vad-thold N   [%-7.2f] voice activity detection "
          "threshold\n",
          params.vad_thold);
  fprintf(stderr,
          "  -fth N,   --freq-thold N  [%-7.2f] high-pass frequency cutoff\n",
          params.freq_thold);
  fprintf(stderr,
          "  -su,      --speed-up      [%-7s] speed up audio by x2 (reduced "
          "accuracy)\n",
          params.speed_up ? "true" : "false");
  fprintf(stderr,
          "  -tr,      --translate     [%-7s] translate from source language "
          "to english\n",
          params.translate ? "true" : "false");
  fprintf(stderr, "  -ps,      --print-special [%-7s] print special tokens\n",
          params.print_special ? "true" : "false");
  fprintf(stderr,
          "  -pe,      --print-energy  [%-7s] print sound energy (for "
          "debugging)\n",
          params.print_energy ? "true" : "false");
  fprintf(stderr,
          "  -p NAME,  --person NAME   [%-7s] person name (for prompt "
          "selection)\n",
          params.person.c_str());
  fprintf(stderr, "  -l LANG,  --language LANG [%-7s] spoken language\n",
          params.language.c_str());
  fprintf(stderr, "  -mw FILE, --model-whisper [%-7s] whisper model file\n",
          params.tts_model_path.c_str());
  fprintf(stderr, "  -ml FILE, --model-llama   [%-7s] llama model file\n",
          params.llm_model_path.c_str());
  fprintf(stderr, "  -s FILE,  --voice TEXT    [%-7s] command for TTS\n",
          params.voice.c_str());
  fprintf(stderr,
          "  --prompt-file FNAME       [%-7s] file with custom prompt to start "
          "dialog\n",
          "");
  fprintf(stderr,
          "  --session FNAME       file to cache model state in (may be "
          "large!) (default: none)\n");
  fprintf(stderr, "  --verbose-prompt          [%-7s] print prompt at start\n",
          params.verbose_prompt ? "true" : "false");
  fprintf(stderr, "\n");
}

bool common::cc_params_from_config(const std::string& fname, CCParams& params) {
  params = CCParams();
  std::ifstream f(fname);
  json data = json::parse(f);
  if (data.contains("prob0")) {
    params.prob0 = data["prob0"];
  }
  if (data.contains("n_threads")) {
    params.n_threads = data["n_threads"];
  }
  if (data.contains("voice_ms")) {
    params.voice_ms = data["voice_ms"];
  }
  if (data.contains("capture_id")) {
    params.capture_id = data["capture_id"];
  }
  if (data.contains("max_tokens")) {
    params.max_tokens = data["max_tokens"];
  }
  if (data.contains("audio_ctx")) {
    params.audio_ctx = data["audio_ctx"];
  }
  if (data.contains("vad_thold")) {
    params.vad_thold = data["vad_thold"];
  }
  if (data.contains("freq_thold")) {
    params.freq_thold = data["freq_thold"];
  }
  if (data.contains("speed_up")) {
    params.speed_up = data["speed_up"];
  }
  if (data.contains("translate")) {
    params.translate = data["translate"];
  }
  if (data.contains("print_special")) {
    params.print_special = data["print_special"];
  }
  if (data.contains("print_energy")) {
    params.print_energy = data["print_energy"];
  }
  if (data.contains("no_timestamps")) {
    params.no_timestamps = data["no_timestamps"];
  }
  if (data.contains("verbose_prompt")) {
    params.verbose_prompt = data["verbose_prompt"];
  }
  if (data.contains("person")) {
    params.person = data["person"];
  }
  if (data.contains("bot_name")) {
    params.bot_name = data["bot_name"];
  }
  if (data.contains("chat_symb")) {
    params.chat_symb = data["chat_symb"];
  }
  if (data.contains("language")) {
    params.language = data["language"];
  }
  if (data.contains("tts_model_path")) {
    params.tts_model_path = data["tts_model_path"];
  }
  if (data.contains("llm_model_path")) {
    params.llm_model_path = data["llm_model_path"];
  }
  if (data.contains("voice")) {
    params.voice = data["voice"];
  }
  if (data.contains("prompt")) {
    params.prompt = data["prompt"];
  }
  if (data.contains("path_session")) {
    params.path_session = data["path_session"];
  }
  return true;
}

bool common::cc_params_parse(int argc, char** argv, CCParams& params) {
  for (int i = 1; i < argc; i++) {
    std::string arg = argv[i];

    if (arg == "-h" || arg == "--help") {
      common::cc_print_params_usage(argc, argv, params);
      exit(0);
    } else if (arg == "-t" || arg == "--threads") {
      params.n_threads = std::stoi(argv[++i]);
    } else if (arg == "-vms" || arg == "--voice-ms") {
      params.voice_ms = std::stoi(argv[++i]);
    } else if (arg == "-c" || arg == "--capture") {
      params.capture_id = std::stoi(argv[++i]);
    } else if (arg == "-mt" || arg == "--max-tokens") {
      params.max_tokens = std::stoi(argv[++i]);
    } else if (arg == "-ac" || arg == "--audio-ctx") {
      params.audio_ctx = std::stoi(argv[++i]);
    } else if (arg == "-vth" || arg == "--vad-thold") {
      params.vad_thold = std::stof(argv[++i]);
    } else if (arg == "-fth" || arg == "--freq-thold") {
      params.freq_thold = std::stof(argv[++i]);
    } else if (arg == "-su" || arg == "--speed-up") {
      params.speed_up = true;
    } else if (arg == "-tr" || arg == "--translate") {
      params.translate = true;
    } else if (arg == "-ps" || arg == "--print-special") {
      params.print_special = true;
    } else if (arg == "-pe" || arg == "--print-energy") {
      params.print_energy = true;
    } else if (arg == "--verbose-prompt") {
      params.verbose_prompt = true;
    } else if (arg == "-p" || arg == "--person") {
      params.person = argv[++i];
    } else if (arg == "--session") {
      params.path_session = argv[++i];
    } else if (arg == "-l" || arg == "--language") {
      params.language = argv[++i];
    } else if (arg == "-mw" || arg == "--model-whisper") {
      params.tts_model_path = argv[++i];
    } else if (arg == "-ml" || arg == "--model-llama") {
      params.llm_model_path = argv[++i];
    } else if (arg == "-v" || arg == "--voice") {
      params.voice = argv[++i];
    } else if (arg == "--prompt-file") {
      std::ifstream file(argv[++i]);
      std::copy(std::istreambuf_iterator<char>(file),
                std::istreambuf_iterator<char>(), back_inserter(params.prompt));
      if (params.prompt.back() == '\n') {
        params.prompt.pop_back();
      }
    } else {
      fprintf(stderr, "error: unknown argument: %s\n", arg.c_str());
      cc_print_params_usage(argc, argv, params);
      exit(0);
    }
  }

  return true;
}
