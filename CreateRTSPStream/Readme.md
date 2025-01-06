# Why we need to stream videos as RTSP ? 

1. deepstream pipeline testing
2. e2e video analysis testing (compare resource utilization including rtsp decoding)
3. CCTV camera interruption tests
4. local demo setup

# How to stream videos as RTSP?

Docs: https://docs.nvidia.com/jetson/jps/nvstreamer/moj-nvstreamer.html


# Nvidia Jetson Platform 

1. Download nvstreamer files from 
https://catalog.ngc.nvidia.com/orgs/nvidia/teams/jps/resources/reference-workflow-and-resources/files

```bash
wget --content-disposition 'https://api.ngc.nvidia.com/v2/resources/org/nvidia/team/jps/reference-workflow-and-resources/2.0.0/files?redirect=true&path=nvstreamer-2.0.0.tar.gz' -O nvstreamer-2.0.0.tar.gz

tar -xvf nvstreamer-2.0.0.tar.gz

cd nvstreamer
```

2. Start NVIDIA Nvstreamer

```bash
docker compose -f compose_nvstreamer.yaml up -d  --force-recreate
```

3. Upload file (H264 format with mp4 and mkv containers)

4. Consume RTSP stream 

```bash
ffplay rtsp://<JETSON_IP>:31554/nvstream/opt/store/nvstreamer_videos/sample_1080p_h264.mp4
```

VLC (Ubuntu 22+)

```bash
sudo apt-get remove -y --purge 'vlc*'
# install the snap package.
sudo snap install vlc

vlc rtsp://<JETSON_IP>:31554/nvstream/opt/store/nvstreamer_videos/sample_1080p_h264.mp4
```


4. Stop Nvstream 

```bash
sudo docker compose -f compose_nvstreamer.yaml down --remove-orphans
```


# Server


1. Download nvstreamer files from 
https://catalog.ngc.nvidia.com/orgs/nvidia/teams/jps/resources/reference-workflow-and-resources/files

```bash
wget --content-disposition 'https://api.ngc.nvidia.com/v2/resources/org/nvidia/team/jps/reference-workflow-and-resources/2.0.0/files?redirect=true&path=nvstreamer-2.0.0.tar.gz' -O nvstreamer-2.0.0.tar.gz

tar -xvf nvstreamer-2.0.0.tar.gz

cd nvstreamer
```

2. Run docker

GPU:
```bash
docker run -e ADAPTOR=streamer -d --restart always --gpus all -v $PWD/streamer_videos:/opt/store/nvstreamer_videos  -v $PWD/config/nvstreamer/:/home/vst/vst_release/updated_configs/ --net=host nvcr.io/nvidia/jps/vst:1.3.0-24.08.1-x86_64 --vstConfigFile /home/vst/vst_release/updated_configs/vst_config.json
```

CPU:
```bash
docker run -e ADAPTOR=streamer -d --restart always -v $PWD/streamer_videos:/opt/store/nvstreamer_videos  -v $PWD/config/nvstreamer/:/home/vst/vst_release/updated_configs/ --net=host nvcr.io/nvidia/jps/vst:1.3.0-24.08.1-x86_64 --vstConfigFile /home/vst/vst_release/updated_configs/vst_config.json
```

3. Open `http://localhost:31000/#/streamer/dashboard` and add a video file to stream


# Open RTSP stream with gstreamer 

```bash
gst-launch-1.0 rtspsrc location=rtsp://192.168.1.66:31554/nvstream/opt/store/nvstreamer_videos/sample_1080p_h264.mp4 latency=100 ! queue ! rtph264depay ! h264parse ! avdec_h264 ! videoconvert ! videoscale ! video/x-raw,width=640,height=480 ! autovideosink
```

# Other RTSP sources

https://github.com/rgl/vlc-rtsp-server

https://github.com/bluenviron/mediamtx


# RTSP test stream 

https://rtsp.stream/posts/opening-rtsp-stream-using-vlc-gstreamer-ffmpeg




# Configurations to adjust 


## VST

number of WebRTC streams (default is 1)