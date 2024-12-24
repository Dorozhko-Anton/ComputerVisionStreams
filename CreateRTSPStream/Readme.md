
Docs: https://docs.nvidia.com/jetson/jps/nvstreamer/moj-nvstreamer.html

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


# Nvidia Jetson Platform 

1. Download nvstreamer files from 
https://catalog.ngc.nvidia.com/orgs/nvidia/teams/jps/resources/reference-workflow-and-resources/files

```bash
wget --content-disposition 'https://api.ngc.nvidia.com/v2/resources/org/nvidia/team/jps/reference-workflow-and-resources/2.0.0/files?redirect=true&path=nvstreamer-2.0.0.tar.gz' -O nvstreamer-2.0.0.tar.gz

tar -xvf nvstreamer-2.0.0.tar.gz
```


2. Upload file (H264 format with mp4 and mkv containers)

3. Consume RTSP stream 

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


# Other RTSP sources

https://github.com/rgl/vlc-rtsp-server
https://github.com/bluenviron/mediamtx


# RTSP test stream 

https://rtsp.stream/posts/opening-rtsp-stream-using-vlc-gstreamer-ffmpeg