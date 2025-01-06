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

(make sure to match CUDA version of nvidia-smi with ngc image)

```
docker login nvcr.io
docker build -t deepstream:7.0 .
```

```
# Step to run the docker
export DISPLAY=:0
xhost +
docker run -it --rm --net=host --gpus all --privileged  -e DISPLAY=$DISPLAY --device /dev/snd -v /tmp/.X11-unix/:/tmp/.X11-unix -v $PWD:/workspace deepstream:7.0 bash
```


# Execute

```bash

cd /workspace/cpp_pipeline/
mkdir build && cd build && cmake .. && make

./my-deepstream-app file:///opt/nvidia/deepstream/deepstream/samples/streams/sample_720p.mp4
```


# Download NVidia Nsight System 

make sure profiler and visualizer having exactly the same version up to minor (otherwise there might be issues with openning of reports)
https://developer.nvidia.com/nsight-systems/get-started

Example:
Nsight Systems 2024.7.1 Full Version
Nsight Systems 2024.7.1 CLI Only

```bash
dpkg -i NsightSystems-linux-cli-public-2024.7.1.84-3512561.deb
```

# Profile pipeline 


```bash 
# generate engine (take a couple of minutes for a first run)
./my-deepstream-app file:///opt/nvidia/deepstream/deepstream/samples/streams/sample_720p.mp4

# save profiling
nsys profile -w true -t cuda,nvtx,osrt -o nsight_report -y 10 -d 10 --force-overwrite true ./my-deepstream-app file:///opt/nvidia/deepstream/deepstream/samples/streams/sample_720p.mp4

```

other parameters 
https://docs.nvidia.com/nsight-systems/UserGuide/index.html 

# Open with UI  



# Customize profiling (Add NVTX ranges) 

https://github.com/NVIDIA/NVTX


1. include 

```c++
#include <nvtx3/nvtx3.hpp>
```

2. modify Cmake file
```cmake

include(CPM.cmake)

CPMAddPackage(
    NAME NVTX
    GITHUB_REPOSITORY NVIDIA/NVTX
    GIT_TAG v3.1.0-c-cpp
    GIT_SHALLOW TRUE)

target_link_libraries(some_cpp_program PRIVATE nvtx3-cpp)
```

3. wget CPM
```
wget https://raw.githubusercontent.com/cpm-cmake/CPM.cmake/refs/heads/master/cmake/CPM.cmake
```


NVTX user Guide 
https://docs.nvidia.com/nsight-systems/UserGuide/index.html
