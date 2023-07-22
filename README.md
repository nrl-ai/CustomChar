# CustomChar - Your customized AI characters

![CustomChar](/docs/CustomChar.svg)

Your customized AI characters on commodity hardware. This project aims to be a framework to enable the creation of AI characters that can be used in games, simulations, and other applications without depending on a specific platform, cloud services, or specialized hardware.

## Features

- [ ] Build AI characters from knowledge database:
  - [x] LLamaCpp for LLM. Chat with Llama-V2 on all platforms.
  - [ ] Knowledge indexing and searching.
- [x] Talk to your character:
  - [x] STT with whisper.cpp. Now supports speech recognition on all platforms.
- [ ] Hear from your character:
  - [x] TTS with "say" on MacOS. **Note**: "say" is not available on other platforms (Windows, Linux)
  - [ ] TTS with bark.cpp. Expected to be available on all platforms.

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
