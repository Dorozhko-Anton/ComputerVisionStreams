# VLM Video Summarization


docs: https://docs.nvidia.com/jetson/jps/inference-services/video_summarization.html


Observations:
1. a lot of video data collected
2. videos contain many sparse but significant events of interest
3. according to some estimations 99% of CCTV cameras videos are never processed in any meaningful way


# on Jetson 

1. Upload video
2. Invoke summarization through API (summarization is done with NanoLLM framework https://www.jetson-ai-lab.com/tutorial_nano-llm.html) 


## Features

1. Event embeddings and clustering 
-    1.1 compute embedding of a video clip with SigLIP
-    1.2 apply DBSCAN to get clusters of events
2. Video Content Retrieval
-    2.1 search for events using language queries
3. Video Summarization
-    3.1 VILA1.5, GPT4 


Optimizations:
1. INT4 AWQ quantized VLMs
2. KV-CACHE
3. Faiss_lite

# Run video summarization service 


## How it works?

1. Split video into chunks
2. Caption batches of frames  (`summarize_vila, summarize_gpt4`)                       - VILA1.5 of GPT-4o
3. Summarize every chunk  (`summaries = _summarize`)                                   - OpenAI gpt-4 (OLLAMA)
4. Summarize all captions into a video summary (`final_summary = caption_integration`) - OpenAI gpt-4 (OLLAMA)


## Prompts 


### System prompt for caption 

1. System prompt 

"You are a helpful assistant."

2. Visiom LM prompt for image catpion: 

"Examine these video frames and provide a detailed, accurate description of observable actions. Focus exclusively on visible activity without making inferences or assumptions. Remember, these frames capture the same person over time, not different individuals. Be concise and precise in your description."

3. Caption summary prompt:

"Provide a high-level summarization of the entire video, focusing on overall activity. Use the provided summaries of continuous video chunks to inform your summary. Do not analyze each scene individually."

4. Whole video Summary 

"Analyze the frame-by-frame descriptions of human activities below. Summarize the continuous activity into a single, cohesive sentence starting with the subject. Captions may contain errors, so ignore captions that are less frequently mentioned in the overall description. Avoid generating unreal or unsure interactions by disregarding any irrelevant information about human activity."


# Setup

## Prerequisite for local inference (OLLAMA)

Use ollama to host OpenAI compatible local LLM (https://ollama.com/download)

```bash
curl -fsSL https://ollama.com/install.sh | sh
sudo systemctl stop ollama
OLLAMA_HOST=0.0.0.0 ollama serve

ollama run llama3.2
```


```bash
curl http://0.0.0.0:11434/v1/chat/completions \
    -H "Content-Type: application/json" \
    -d '{
        "model": "llama3.2",
        "messages": [
            {
                "role": "system",
                "content": "You are a helpful assistant."
            },
            {
                "role": "user",
                "content": "Why 42 is a meaning of life?"
            }
        ]
    }'
```

## Start Video summarization service

1. Run redis service
```
sudo systemctl start jetson-redis
```

2. Create config

```bash
{
    "api_server_port": 19000,
    "redis_host": "localhost",
    "redis_port": 6379,
    "redis_stream": "test",
    "redis_output_interval": 60,
    "log_level": "INFO",
    "jetson_ip": "localhost",
    "video_port": 81,
    "max_images": 8,
    "ingress_port": 30080,
    "streamer_port": 31000,
    "segment_time": 20,
    "openai_api_key": "ollama",
    "base_url" : "http://192.168.1.51:11434/v1",
    "model" : "llama3.2"
}

```

3. Start summarization container with updated OpenAI client to support OLLAMA openai compatible server

```bash
docker pull nvcr.io/nvidia/jps/vlm_summarization:2.0.8
docker build -t vlm_summarization:2.0.8 .


docker run -itd --runtime nvidia --network host -v ./config/main_config.json:/configs/main_config.json -v /data/vsm-videos/:/data/videos/ -v ./main.py:/video_summarization/src/main.py -v ./config.py:/video_summarization/src/config.py  -v ./models:/root/.cache/huggingface -e CONFIG_PATH="/configs/main_config.json"  --name vlmsum vlm_summarization:2.0.8
```

modify: 
1. config
2. data location (`/data/videos`)
3. Check that service is up 

```bash
docker logs -f vlmsum
# wait when model will be quantized and served (a couple of minutes)
# check 
#  Uvicorn running on http://0.0.0.0:19000 (Press CTRL+C to quit)
```

docs: http://192.168.1.66:19000/docs

5. Upload video files

```bash
curl -X POST "http://192.168.1.66:19000/files?filepath=/data/videos/sample_1080p_h264.mp4"
# filepath is based on where the file is within the container.
```

6. Retrieve ID

```bash
VIDEO_ID=`curl -X GET "http://192.168.1.66:19000/files" | jq 'keys'[0] | tr -d '"'`
```

6. Summarize

```bash
curl -X POST "http://192.168.1.66:19000/summarize" \
    -H "accept: application/json" \
    -H "Content-Type: application/json" \
    -d "{\"stream\": false, \"id\": \"$VIDEO_ID\", \"model\": \"vila-1.5\", \"chunk_duration\": 20}"
```

check progress
```bash
docker logs -f vlmsum
```

7. Fetch summary 

```bash
curl -X GET "http://192.168.1.66:19000/summarize?stream=false&id=$VIDEO_ID"
```


# on Server 

https://build.nvidia.com/nvidia/video-search-and-summarization/blueprintcard



