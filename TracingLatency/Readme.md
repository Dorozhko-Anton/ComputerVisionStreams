# Docker installation

https://docs.nvidia.com/metropolis/deepstream/dev-guide/text/DS_docker_containers.html 

1. install `docker`
2. install `nvidia-container-toolkit` 
3. get `NGC` account
4. get `NGC_API_KEY`  
5. `docker login nvcr.io`

```bash
Username: "$oauthtoken"
Password: "YOUR_NGC_API_KEY"
```

# Run docker with deepstream

https://catalog.ngc.nvidia.com/orgs/nvidia/containers/deepstream 

```
docker login nvcr.io
docker build -t deepstream:7.0-latency-tracing .
```

```
# Step to run the docker
export DISPLAY=:0
xhost +
docker run -it --rm --net=host --gpus all -e DISPLAY=$DISPLAY --device /dev/snd -v /tmp/.X11-unix/:/tmp/.X11-unix -v $PWD:/workspace deepstream:7.0-latency-tracing bash
```
## Requirements

```bash
cd /workspace/deepstream_pipeline/
pip install -r requirements.txt
```


# Gstreamer Tracing 

https://gstreamer.freedesktop.org/documentation/additional/design/tracing.html?gi-language=c 


# Measure gst-launch-1.0 pipeline latencies 

```bash


GST_TRACERS="latency(flags=pipeline)" GST_DEBUG=GST_TRACER:7 gst-launch-1.0 videotestsrc num-buffers=5000 ! videoconvert  ! avenc_h263p ! fakesink
GST_TRACERS="latency(flags=element)" GST_DEBUG=GST_TRACER:7 gst-launch-1.0 videotestsrc num-buffers=5000 ! videoconvert  ! avenc_h263p ! fakesink
GST_TRACERS="latency(flags=pipeline+element)" GST_DEBUG=GST_TRACER:7 gst-launch-1.0 videotestsrc num-buffers=5000 ! videoconvert  ! avenc_h263p ! fakesink


GST_DEBUG_COLOR_MODE=off GST_TRACERS="latency(flags=pipeline+element)" GST_DEBUG=GST_TRACER:7 GST_DEBUG_FILE=traces.log gst-launch-1.0 videotestsrc num-buffers=5000 ! videoconvert  ! avenc_h263p ! fakesink



python parse_latency_trace.py traces.log
```


# Measure custom deepstream app latencies

```bash
cd /workspace/deepstream_pipeline/
mkdir -p build && cd build && cmake .. && make

./deepstream_pipeline
```


```bash
GST_DEBUG_COLOR_MODE=off GST_TRACERS="latency(flags=pipeline+element)" GST_DEBUG=GST_TRACER:7 GST_DEBUG_FILE=app_traces.log ./deepstream_pipeline

python ../parse_latency_trace.py app_traces.log
```


# Links:

JS version : https://github.com/podborski/GStreamerLatencyPlotter 