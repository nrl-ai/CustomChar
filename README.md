# CustomChar - Your customized AI characters

![](/docs/banner.svg)

- **Repository:** [https://github.com/nrl-ai/CustomChar](https://github.com/nrl-ai/CustomChar).
- **Built With:** [GGML](https://github.com/ggerganov/ggml), [llama.cpp](https://github.com/ggerganov/llama.cpp) (LLM, with [LLaMA-v2](https://ai.meta.com/llama/)), [whisper.cpp](https://github.com/ggerganov/whisper.cpp) (speech recognition).

Your customized AI characters - personal assistants on any hardware! This project aims to be a framework to enable the creation of AI characters that can be used in games, simulations, virtual assistants, and other applications without depending on specific platforms, cloud services, or specialized hardware.

![](docs/customchar.png)

**Some of the applications you can build with CustomChar:**

- Game characters that can talk to you and interact with you.
- Your customized virtual assistant. Think about a [JARVIS](https://en.wikipedia.org/wiki/J.A.R.V.I.S.) version on your computer.
- Online education experience with virtual friends/teachers.
- Cloud services that enable users to create their own virtual characters.

**Our very early experiment with CustomChar**

https://github.com/nrl-ai/CustomChar/assets/18329471/b035afb5-4440-4943-a0d9-e6b3d9f0f315

**Think about your own JARVIS assistant that you can have totally inside your laptop...**

https://github.com/nrl-ai/CustomChar/assets/18329471/005a2d0f-df32-4493-a391-59757d06d9a2

**Note:** This project is still in the early stage of development. We are working hard to make it available as soon as possible. Star this project 🌟🌟🌟 to follow our progress and encourage us to make it happen.

## 1. Architecture

![CustomChar Architecture](/docs/architecture.svg)

## 2. Updates

- 🔥 **2023-08-06:** Load character configuration from a file.
- 🔥 **2023-07-30:** Record a video with "record a video" and "stop recording".
- 🔥 **2023-07-30:** Added OpenCV + camera view.
- 🔥 **2023-07-29:** Added a plugin executor to open apps.
- 🔥 **2023-07-25:** Added UI with Dear ImGui.
- 🔥 **2023-07-23:** Documentation website is online at <https://docs.customchar.com>.
- 🔥 **2023-07-22:** Voice chat with your Llama-V2 model. TTS is supported on MacOS only.

## 3. Features

- [ ] Build AI characters from the knowledge database 💬💬💬 :
  - [x] LLamaCpp for LLM. Chat with **Llama-V2** on **all platforms**.
  - [x] Knowledge indexing and searching (EmbedDB + SQLite).
  - [ ] Search knowledge DB to add into prompt.
- [x] Talk to your character 🎤🎤🎤 :
  - [x] STT with **whisper.cpp**. Now supports speech recognition **on all platforms**.
- [ ] Hear from your character 🔊🔊🔊 :
  - [x] TTS with "say" on MacOS. **Note**: "say" is not available on other platforms (Windows, Linux)
  - [ ] TTS with bark.cpp. Expected to be available on all platforms.
- [x] Plugin executor:
  - [x] Open apps
  - [x] Record video
- [ ] Perception engine to see the world through the camera 📷📷📷"
  - [x] Support video capturing
  - [ ] Object detection
- [x] Add GUIs for the character interaction.
  - [x] Dear ImgUI
  - [ ] Web-based GUI
- [ ] Add a GUI for the character creation.
- [ ] Create virtual characters that can be used in games, simulations, and other applications.
  - [x] Iron Man (WIP)
  - [ ] Batman
  - [ ] Superman
- [ ] Add bindings to other languages:
  - [ ] Python
  - [ ] C# (Unity)
  - [ ] Javascript (On-browser)

## 4. Build

Clone the repository:

```bash
git clone https://github.com/nrl-ai/CustomChar --recursive
cd CustomChar
```

Install dependencies:

- [CMake](https://cmake.org/download/)
- C++ 14 compiler

On macOS: Using [Homebrew](https://brew.sh/)

```bash
brew install sdl2 glew glfw3
brew install opencv
brew install ffmpeg # For video recording
```

On Ubuntu:

```bash
sudo apt-get install libsdl2-dev libglew-dev libglfw3-dev
sudo apt-get install libopencv-dev
sudo apt-get install ffmpeg # For video recording
```

On Windows: Using [vcpkg](https://github.com/microsoft/vcpkg), [Git Bash](https://git-scm.com/downloads), and [FFmpeg](https://www.wikihow.com/Install-FFmpeg-on-Windows).

```bash
vcpkg install sdl2:x64-windows glew:x64-windows glfw3:x64-windows
vcpkg install opencv[contrib,nonfree,ffmpeg,ipp]:x64-windows --recurse
```

Build the **CustomChar** executable:

```bash
mkdir build
cd build
cmake ..
make
```

## 5. Usage

Download the models before running the program:

```bash
bash scripts/download-models.sh
```

Run the program:

```bash
./customchar
```

## 6. Build documentation

- **Step 1:** Install **doxygen** first.

- **Step 2:** Build the documentation:

```bash
doxygen Doxyfile.in
```

- **Step 3:** Deploy html documentation from `docs/_build/html`.

- **Step 4:** Our latest documentation is deployed at <https://docs.customchar.com>.

## 7. Contribution

We welcome all contributions to this project.

- For coding style, please follow the style of the existing code.
- Install [clang-format](https://clang.llvm.org/docs/ClangFormat.html) for auto formatting the code.
- Install [pre-commit](https://pre-commit.com/) for the auto-formatting hook or manually run the script `scripts/format-code.sh` to format the code.

```bash
pre-commit install # Install pre-commit hook
bash scripts/format-code.sh # Format code (C++ styling)
```

## 8. Star history

[![Star History Chart](https://api.star-history.com/svg?repos=nrl-ai/CustomChar&type=Date)](https://star-history.com/#nrl-ai/CustomChar)

## 9. References

- Spinner: <https://github.com/dalerank/imspinner>.
- GGML: <https://ggml.ai/>.
- llama.cpp: <https://github.com/ggerganov/llama.cpp>.
- whisper.cpp: <https://github.com/ggerganov/whisper.cpp>.
- LLaMA-v2: <https://ai.meta.com/llama/>.
