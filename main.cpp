#include "customchar/llm.h"
#include "customchar/speech_recognizer.h"
#include "customchar/voice_recoder.h"

#include "whisper-cpp/examples/common.h"

#include <cassert>
#include <cstdio>
#include <fstream>
#include <regex>
#include <string>
#include <thread>
#include <vector>

using namespace CC;

int main(int argc, char ** argv) {
    SpeechRecognizer speech_recognizer(argc, argv);
    VoiceRecoder voice_recoder;

    // Init LLM session
    LLM llm;
    std::string path_session = "";
    if (!path_session.empty()) {
        llm.load_session(path_session);
    }
    llm.eval_model();

    // Start talking
    printf("Start speaking in the microphone\n");
    printf("%s%s", params.person.c_str(), chat_symb.c_str());
    fflush(stdout);

    // Clear audio buffer to avoid processing old audio
    voice_recoder.clear_audio_buffer();

    const bot_name = "CustomChar";
    const chat_symb = ":";

    // Text inference variables
    const int voice_id = 2;
    const int n_keep   = embd_inp.size();
    const int n_ctx    = llama_n_ctx(ctx_llama);

    int n_past = n_keep;
    int n_prev = 64;
    int n_session_consumed = !path_session.empty() && session_tokens.size() > 0 ? session_tokens.size() : 0;

    std::vector<llama_token> embd;

    // Reverse prompts for detecting when it's time to stop speaking
    std::vector<std::string> antiprompts = {
        params.person + chat_symb,
    };

    int n_iter = 0;
    bool is_running  = true;
    while (is_running) {
        // Handle Ctrl + C
        is_running = sdl_poll_events();
        if (!is_running) {
            break;
        }

        // Delay
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        float prob = 0.0f;
        int64_t t_ms = 0;

        // Get audio
        voice_recoder.sample_audio();
        if (!voice_recoder.finished_talking()) {
            continue;
        }

        std::vector<float> audio_buff;
        voice_recoder.get_audio(audio_buff);

        // Recognize speech
        std::string text_heard = speech_recognizer.recognize(audio_buff, prob, t_ms);

        // Tokenize user input
        embd = llm.tokenize(text_heard, false);

        // Skip if nothing was heard
        if (text_heard.empty() || embd.empty()) {
            printf("Heard nothing, skipping ...\n");
            audio.clear();
            continue;
        }

        // Print user input
        text_heard.insert(0, 1, ' ');
        text_heard += "\n" + bot_name + chat_symb;
        fprintf(stdout, "%s%s%s", "\033[1m", text_heard, "\033[0m");
        fflush(stdout);

        // Get answer from LLM
        std::string text_to_speak = llm.get_answer(embd);

        // Replace all \n with space
        text_to_speak = ::replace(text_to_speak, "\n", " ");

        // Remove quotes and special characters to put into command line string
        text_to_speak = ::replace(text_to_speak, "\"", " ");
        text_to_speak = ::replace(text_to_speak, "\\", " ");
        text_to_speak = ::replace(text_to_speak, "\'", " ");
        text_to_speak = ::replace(text_to_speak, "(", " ");
        text_to_speak = ::replace(text_to_speak, ")", " ");

        // Play speak
        std::string command = params.speak + " " + std::to_string(voice_id) + " \"" + text_to_speak + "\"";
        system(command.c_str());

        voice_recoder.clear_audio_buffer();
        ++n_iter;
    }

    return 0;
}
