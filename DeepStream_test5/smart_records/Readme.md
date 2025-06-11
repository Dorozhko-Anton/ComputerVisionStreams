# Build smart record plugin

```bash
docker build -t deepstream:7.0 .
# Step to run the docker
export DISPLAY=:0
xhost +
docker run -it --rm --net=host --gpus all -e DISPLAY=$DISPLAY --device /dev/snd -v /tmp/.X11-unix/:/tmp/.X11-unix -v $PWD:/workspace deepstream:7.0 bash
```


```bash
cd /workspace

apt update 
apt install libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev
CUDA_VER=12.4 make
```

run 

```bash
export GST_PLUGIN_PATH=$GST_PLUGIN_PATH:/workspace/smart-record-plugin

GST_DEBUG=*:3 gst-launch-1.0 filesrc location=/opt/nvidia/deepstream/deepstream/samples/streams/sample_720p.h264 ! h264parse ! nvv4l2decoder !     identity name=rec ! nvvideoconvert ! nveglglessink

gst-launch-1.0 filesrc location=/opt/nvidia/deepstream/deepstream/samples/streams/sample_720p.h264 ! h264parse ! nvv4l2decoder ! \
    recordplugin name=rec ! nvvideoconvert ! nveglglessink
```


run app with signals 

```bash
cd /workspace/
mkdir build && cd build && cmake .. && make

./OldDeepstream
```

gst-launch-1.0 videotestsrc num-buffers=150 ! videoconvert ! x264enc ! mp4mux ! filesink location=test.mp4


# Inspect Deepstream test 5 app 

1. Run DS container 

```
docker run --runtime nvidia -it --rm --name ds-dev nvcr.io/nvidia/deepstream:7.0-samples-multiarch bash
```

2. Connect to container to browse DS code in vs code

dev containers -> attach to running container

open `service-maker/sources/apps/deepstream_test5_app/deepstream_test5.cpp`


# Smart Record Service Maker 

Can be activated by 

1. signal
2. kafka message


```c++

      // create the smart recording action
      auto object = CommonFactory::getInstance().createObject("smart_recording_action", "sr_action");
      auto* sr_action = dynamic_cast<SignalEmitter*>(object.get());
      if (!sr_action) {
        std::cerr << "Failed to create signal emitter" << std::endl;
        return -1;
      }

      sr_action->set(
          "proto-lib", kafka_proto_lib_path,
          "conn-str", msgbroker_conn_str,
          "msgconv-config-file", msgconv_config_path,
          "proto-config-file", msgbroker_config_path,
          "topic-list", "test5-sr");


      for (size_t i = 0; i < source_config.nSources(); i++)
        {
          std::string src_name = "src_";
          src_name += std::to_string(i);
          sr_action->attach("start-sr", pipeline[src_name]);
          sr_action->attach("stop-sr", pipeline[src_name]);
          pipeline[src_name].connectSignal(
              "smart_recording_signal", "sr", "sr-done");
        }

```