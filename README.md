# CustomChar

![CustomChar](CustomChar.svg)

Your customized AI characters on commodity hardware. This project aims to be a framework to enable the creation of AI characters that can be used in games, simulations, and other applications without depending on a specific platform, cloud servicesm, or specialized hardware.

## Features

- [ ] Build AI characters from knowledge database:
  - [ ] LLamaCpp for LLM
  - [ ] Knowledge indexing and searching
- [ ] Hear from your character:
  - [ ] TTS with bark.cpp
- [ ] Talk to your character
  - [ ] STT with whisper.cpp

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
bash download-models.sh
```

Run the program:

```bash
./try-customchar
```
