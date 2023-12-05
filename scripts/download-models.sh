mkdir models

echo "Downloading LLama-2.7B-Chat-GGMLv3 Q4=0 model..."
wget -O models/nous-hermes-llama-2-7b.Q4_0.gguf https://huggingface.co/TheBloke/Nous-Hermes-Llama-2-7B-GGUF/resolve/main/nous-hermes-llama-2-7b.Q4_0.gguf?download=true

echo "Download Whisper models..."
bash scripts/download-whisper-models.sh base.en
