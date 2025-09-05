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
docker build -t deepstream:7.0 .
```

```
# Step to run the docker
export DISPLAY=:0
xhost +
docker run -it --rm --net=host --gpus all -e DISPLAY=$DISPLAY --device /dev/snd -v /tmp/.X11-unix/:/tmp/.X11-unix -v $PWD:/workspace deepstream:7.0 bash
```


# Execute

```bash


cd /workspace/cpp_pipeline_old/
mkdir build && cd build && cmake .. && make

./OldDeepstream
```

# NvDSAnalytics Use-cases 

requires nvinfer
[roi-filtering-stream-0]
[overcrowding-stream-0]          - roi count + threshold

requires nvtracker
[line-crossing-stream-0]         - count, tripwire
[direction-detection-stream-0]   - add direction metadata to tracked objects


# Web UI for nvdsanalytics 

## Get first frame for labelling 

```
gst-launch-1.0 nvurisrcbin uri=file:///opt/nvidia/deepstream/deepstream/samples/streams/sample_720p.mp4 ! nvvideoconvert ! pngenc ! multifilesink location=frame_%d.png max-files=1
```
