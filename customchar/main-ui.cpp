#include <stdio.h>
#include <iostream>
#include <mutex>
#include <thread>

#include <semaphore>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#if defined(__APPLE__)
#define GL_SILENCE_DEPRECATION  // Silence deprecation warnings on macos
#endif
#include <GLFW/glfw3.h>  // Will drag system OpenGL headers

#include "chat_history.h"
#include "chat_message.h"

#include "customchar/character.h"
#include "customchar/common.h"
#include "customchar/helpers.h"

using namespace CC;

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to
// maximize ease of testing and compatibility with old VS compilers. To link
// with VS2010-era libraries, VS2015+ requires linking with
// legacy_stdio_definitions.lib, which we do using this pragma. Your own project
// should not be affected, as you are likely to link with a newer binary of GLFW
// that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && \
    !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

// Global variables
// synchronize
// everytime user sends message, IMGUI sets global variable to message
// signal client server ... lock/unlock mutex
constexpr int TEXT_MESSAGE_SIZE = 1024 * 8;
constexpr int INIT_WINDOW_WIDTH = 720;
constexpr int INIT_WINDOW_HEIGHT = 400;

static void glfw_error_callback(int error, const char* description) {
  fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

std::mutex mtx;
bool on_new_message(const std::string& text, const std::string& sender,
                    std::shared_ptr<ChatHistory> history) {
  mtx.lock();
  history->add_message(text, sender);
  mtx.unlock();
  return true;
}

/**
 * @brief Main ImGUI loop
 */
void runImgui(std::shared_ptr<ChatHistory> history) {
  // Setup window
  glfwSetErrorCallback(glfw_error_callback);
  if (!glfwInit()) return;

// Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
  // GL ES 2.0 + GLSL 100
  const char* glsl_version = "#version 100";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
  // GL 3.2 + GLSL 150
  const char* glsl_version = "#version 150";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
  // GL 3.0 + GLSL 130
  const char* glsl_version = "#version 130";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+
  // only glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // 3.0+ only
#endif

  // Create window with graphics context
  GLFWwindow* window = glfwCreateWindow(INIT_WINDOW_WIDTH, INIT_WINDOW_HEIGHT,
                                        "CustomChar", NULL, NULL);
  if (window == NULL) return;
  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);  // Enable vsync

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  (void)io;
  // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable
  // Keyboard Controls io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; //
  // Enable Gamepad Controls

  // Setup Dear ImGui style
  // ImGui::StyleColorsDark();
  ImGui::StyleColorsDark();

  // Setup Platform/Renderer backends
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init(glsl_version);

  // Load Fonts
  // - If no fonts are loaded, dear imgui will use the default font. You can
  // also load multiple fonts and use ImGui::PushFont()/PopFont() to select
  // them.
  // - AddFontFromFileTTF() will return the ImFont* so you can store it if you
  // need to select the font among multiple.
  // - If the file cannot be loaded, the function will return NULL. Please
  // handle those errors in your application (e.g. use an assertion, or
  // display an error and quit).
  // - The fonts will be rasterized at a given size (w/ oversampling) and
  // stored into a texture when calling
  // ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame
  // below will call.
  // - Read 'docs/FONTS.md' for more instructions and details.
  // - Remember that in C/C++ if you want to include a backslash \ in a string
  // literal you need to write a double backslash \\ !
  // io.Fonts->AddFontDefault();
  // io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
  // io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
  // io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
  // io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
  // ImFont* font =
  // io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f,
  // NULL, io.Fonts->GetGlyphRangesJapanese()); IM_ASSERT(font != NULL);

  // Our state
  ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
  bool justSent = true;

  // Initial text
  char text[TEXT_MESSAGE_SIZE] = "";

  // Main loop
  while (!glfwWindowShouldClose(window)) {
    // Poll and handle events (inputs, window resize, etc.)
    // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard
    // flags to tell if dear imgui wants to use your inputs.
    // - When io.WantCaptureMouse is true, do not dispatch mouse input
    // data to your main application, or clear/overwrite your copy of
    // the mouse data.
    // - When io.WantCaptureKeyboard is true, do not dispatch keyboard
    // input data to your main application, or clear/overwrite your copy
    // of the keyboard data. Generally you may always pass all inputs to
    // dear imgui, and hide them from your application based on those
    // two flags
    glfwPollEvents();

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    /**
     * @brief Shows connection window if not connected, otherwise show
     * basic chat window
     */

    // Is connected
    int TEXTBOX_HEIGHT = ImGui::GetTextLineHeight() * 4;

    // Make window take up full system window
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(io.DisplaySize);

    // Create window
    ImGui::Begin("CustomChar", NULL,
                 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                     ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);

    // Child window scrollable area
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;

    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
    ImGui::BeginChild(
        "ChildR",
        ImVec2(0, ImGui::GetContentRegionAvail().y - TEXTBOX_HEIGHT - 20), true,
        window_flags);

    // TODO: Format chat history
    ImGui::Dummy(ImVec2(0, ImGui::GetContentRegionAvail().y));

    for (ChatMessage message : history->get_char_history()) {
      ImGui::Spacing();
      // ImGui::TextWrapped("%s", message.get_time().c_str());
      ImGui::TextWrapped("> %s: %s", message.get_sender().c_str(),
                         message.get_message().c_str());
    }
    if (history->has_new_message() || justSent) {
      ImGui::SetScrollHereY(1.0f);
    }

    ImGui::EndChild();
    ImGui::PopStyleVar();

    // Text input area flags
    // Prevent user from inputting characters
    ImGuiInputTextFlags input_flags = ImGuiInputTextFlags_ReadOnly;

    // Refocus text area if text was just sent
    if (justSent) {
      ImGui::SetKeyboardFocusHere();
      justSent = false;
    }

    // Create text area and send button
    strcpy(text, "Say something...");
    if (ImGui::InputTextMultiline("##source", text, IM_ARRAYSIZE(text),
                                  ImVec2(-FLT_MIN, TEXTBOX_HEIGHT),
                                  input_flags)) {
      justSent = on_new_message(text, "User", history);
    };

    ImGui::End();

    // Rendering
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w,
                 clear_color.z * clear_color.w, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
  }

  std::cout << "Main ImGUI loop ended" << std::endl;

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(window);
  glfwTerminate();
}

int main(int argc, char** argv) {
  // Parse command line arguments
  CCParams params;
  if (cc_params_parse(argc, argv, params) == false) {
    exit(1);
  }
  if (whisper_lang_id(params.language.c_str()) == -1) {
    fprintf(stderr, "error: unknown language '%s'\n", params.language.c_str());
    cc_print_usage(argc, argv, params);
    exit(1);
  }

  // Initialize chat history
  std::shared_ptr<ChatHistory> history = std::make_shared<ChatHistory>();

  // Create character
  Character character(params);

  // Set message callbacks
  character.set_on_user_message(
      std::bind(on_new_message, std::placeholders::_1, "User", history));
  character.set_on_bot_message(
      std::bind(on_new_message, std::placeholders::_1, "CustomChar", history));

  // Start character in a new thread
  std::thread character_thread(&Character::run, &character);
  character_thread.detach();

  // Main GUI loop
  runImgui(history);

  return 0;
}
