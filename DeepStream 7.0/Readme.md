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

cd /workspace/cpp_pipeline/
mkdir build && cd build && cmake .. && make

./my-deepstream-app file:///opt/nvidia/deepstream/deepstream/samples/streams/sample_720p.mp4


cd /workspace/cpp_pipeline_old/
mkdir build && cd build && cmake .. && make

./OldDeepstream

```