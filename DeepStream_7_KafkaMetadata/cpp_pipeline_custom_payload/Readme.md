
# Custom NvMsgConv plugin 


dependencies 

```bash
apt-get install libglib2.0-dev libjson-glib-dev uuid-dev libyaml-cpp-dev
```

build custom nvmsgconv

```bash
cd /workspace/cpp_pipeline_custom_payload/nvmsgbroker_custom/nvmsgconv
make install 
```

build and run pipeline

```bash
cd /workspace/cpp_pipeline_custom_payload/build
make 

GST_DEBUG=*:3  ./custom-payload file:///opt/nvidia/deepstream/deepstream/samples/streams/sample_720p.mp4
```








# link
https://github.com/NVIDIA-AI-IOT/deepstream-retail-analytics/tree/main/nvmsgconv



