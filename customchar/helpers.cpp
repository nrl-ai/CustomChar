#include "customchar/helpers.h"

void CC::cc_print_usage(int /*argc*/, char** argv, const CCParams& params) {
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
          params.sr_model_path.c_str());
  fprintf(stderr, "  -ml FILE, --model-llama   [%-7s] llama model file\n",
          params.llm_model_path.c_str());
  fprintf(stderr, "  -s FILE,  --speak TEXT    [%-7s] command for TTS\n",
          params.speak.c_str());
  fprintf(stderr,
          "  --prompt-file FNAME       [%-7s] file with custom prompt to start "
          "dialog\n",
          "");
  fprintf(stderr,
          "  --session FNAME       file to cache model state in (may be "
          "large!) (default: none)\n");
  fprintf(stderr, "  --verbose-prompt          [%-7s] print prompt at start\n",
          params.verbose_prompt ? "true" : "false");
  fprintf(stderr, "  -f FNAME, --file FNAME    [%-7s] text output file name\n",
          params.fname_out.c_str());
  fprintf(stderr, "\n");
}

bool CC::cc_params_parse(int argc, char** argv, CCParams& params) {
  for (int i = 1; i < argc; i++) {
    std::string arg = argv[i];

    if (arg == "-h" || arg == "--help") {
      CC::cc_print_usage(argc, argv, params);
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
      params.sr_model_path = argv[++i];
    } else if (arg == "-ml" || arg == "--model-llama") {
      params.llm_model_path = argv[++i];
    } else if (arg == "-s" || arg == "--speak") {
      params.speak = argv[++i];
    } else if (arg == "--prompt-file") {
      std::ifstream file(argv[++i]);
      std::copy(std::istreambuf_iterator<char>(file),
                std::istreambuf_iterator<char>(), back_inserter(params.prompt));
      if (params.prompt.back() == '\n') {
        params.prompt.pop_back();
      }
    } else if (arg == "-f" || arg == "--file") {
      params.fname_out = argv[++i];
    } else {
      fprintf(stderr, "error: unknown argument: %s\n", arg.c_str());
      cc_print_usage(argc, argv, params);
      exit(0);
    }
  }

  return true;
}
