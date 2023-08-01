#!/bin/bash

# Create models directory if it doesn't exist
[ ! -d "models" ] && mkdir models

# Function for downloading a file if it doesn't exist
download_file() {
    FILE_PATH=$1
    URL=$2

    if [ -f "$FILE_PATH" ]; then
        echo "File $FILE_PATH already exists, skipping download."
    else
        echo "Downloading $FILE_PATH..."
        wget -O $FILE_PATH $URL
    fi
}

# Download files
download_file "models/llama-2-7b-chat.ggmlv3.q4_0.bin" "https://huggingface.co/TheBloke/Llama-2-7B-Chat-GGML/resolve/main/llama-2-7b-chat.ggmlv3.q4_0.bin"
[ $? -eq 0 ] || exit 1  # If the previous command (download_file) failed, exit the script with an error status

# Assuming scripts/download-whisper-models.sh base.en downloads files to "models" directory
download_file "models/base.en" "http://example.com/path/to/base.en"  # replace with the actual URL
[ $? -eq 0 ] || exit 1  # If the previous command (download_file) failed, exit the script with an error status
