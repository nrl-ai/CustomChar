mkdir models

echo "Downloading LLama-2.7B-Chat-GGMLv3 Q4=0 model..."
wget -O models/llama-2-7b-chat.ggmlv3.q4_0.bin https://huggingface.co/TheBloke/Llama-2-7B-Chat-GGML/resolve/main/llama-2-7b-chat.ggmlv3.q4_0.bin

echo "Download Whisper models..."
bash download-whisper-models.sh base.en
