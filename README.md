# CustomChar - Your customized AI characters

![](/docs/banner.svg)

**Repository:** [https://github.com/vietanhdev/CustomChar](https://github.com/vietanhdev/CustomChar).

Your customized AI characters on commodity hardware. This project aims to be a framework to enable the creation of AI characters that can be used in games, simulations, and other applications without depending on a specific platform, cloud services, or specialized hardware.

**Note:** This project is still in the early stage of development. We are working hard to make it available as soon as possible. Star this project 🌟 to follow our progress and encourage us to make it happen.

## Architecture

![CustomChar Architecture](/docs/architecture.svg)

## Updates

- 🔥 **2023-07-23:** Documentation website is online at <https://docs.customchar.com>.
- 🔥 **2023-07-22:** Voice chat with your Llama-V2 model. TTS is supported on MacOS only.

## Features

- [ ] Build AI characters from knowledge database 💬💬💬 :
  - [x] LLamaCpp for LLM. Chat with Llama-V2 on all platforms.
  - [ ] Knowledge indexing and searching.
- [x] Talk to your character 🎤🎤🎤 :
  - [x] STT with whisper.cpp. Now supports speech recognition on all platforms.
- [ ] Hear from your character 🔊🔊🔊 :
  - [x] TTS with "say" on MacOS. **Note**: "say" is not available on other platforms (Windows, Linux)
  - [ ] TTS with bark.cpp. Expected to be available on all platforms.
- [ ] Perception engine to see the world through the camera 📷📷📷.
- [ ] 3D visualization of the character.
- [ ] Add a GUI for the character interaction.
- [ ] Add a GUI for the character creation.
- [ ] Create virual characters that can be used in games, simulations, and other applications.
  - [ ] Elon Musk
  - [ ] Iron Man
  - [ ] Batman
  - [ ] Superman
- [ ] Add bindings to other languages:
  - [ ] Python
  - [ ] C# (Unity)
  - [ ] Javscript (On-browser)

## Build

**CustomChar** depends on SDL2 library to capture audio from the microphone. Install SDL2 library before building.

```bash
# Install SDL2 on Linux
sudo apt-get install libsdl2-dev

# Install SDL2 on Mac OS
brew install sdl2
```

Build the **CustomChar** executable:

```bash
mkdir build
cd build
cmake ..
make
```

## Usage

Download the models before running the program:

```bash
bash scripts/download-models.sh
```

Run the program:

```bash
./customchar
```

## Build documentation

- Step 1: Install **doxygen** first.

- Step 2: Build the documentation:

```bash
doxygen Doxyfile.in
```

- Step 3: Deploy html documentation from `docs/_build/html`.

- Step 4: Our lastest documentation is deployed at <https://docs.customchar.com>.

## Contribution

We welcome all contributions to this project.

- For coding style, please follow the style of the existing code.
- We use format tool for auto formating the code. You can use the following command to format code before commiting:

```bash
pre-commit install # Install pre-commit hook
bash scripts/format-code.sh # Format code (C++ styling)
```

[![Star History Chart](https://api.star-history.com/svg?repos=vietanhdev/CustomChar&type=Date)](https://star-history.com/#vietanhdev/CustomChar)
