

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

```
docker login nvcr.io
docker build -t deepstream:base .
```

```
# Step to run the docker
export DISPLAY=:0
xhost +
docker run -it --rm --net=host --gpus all -e DISPLAY=$DISPLAY --device /dev/snd -v /tmp/.X11-unix/:/tmp/.X11-unix deepstream:base
```


# Execute

Run commands from `commands.sh`