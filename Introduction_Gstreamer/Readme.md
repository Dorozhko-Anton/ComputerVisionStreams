

# Run docker with gstreamer

```bahs
docker build -t gstreamer:base .
```

```
# Step to run the docker
export DISPLAY=:0
xhost +
docker run -it --rm --net=host --gpus all -e DISPLAY=$DISPLAY -p 8888:8888 --device /dev/snd -v /tmp/.X11-unix/:/tmp/.X11-unix gstreamer:base
```


# Execute

Run commands from `commands.sh`