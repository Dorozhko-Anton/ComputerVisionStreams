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

# Run local kafka setup for dev


# Kafka minimal setup

https://github.com/provectus/kafka-ui/blob/master/documentation/compose/kafka-ui.yaml

https://docs.nvidia.com/metropolis/deepstream/dev-guide/text/DS_plugin_gst-nvmsgbroker.html#configuring-protocol-settings

```
docker compose up -d
```
open UI http://0.0.0.0:8080 

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

mkdir debug-payload
./my-deepstream-app file:///opt/nvidia/deepstream/deepstream/samples/streams/sample_720p.mp4

GST_DEBUG=*:3 ./my-deepstream-app file:///opt/nvidia/deepstream/deepstream/samples/streams/sample_720p.mp4



cd /workspace/cpp_pipeline/
mkdir build && cd build && cmake .. && make

mkdir debug-payload
./my-deepstream-app file:///opt/nvidia/deepstream/deepstream/samples/streams/sample_720p.mp4

GST_DEBUG=*:3 ./custom-payload file:///opt/nvidia/deepstream/deepstream/samples/streams/sample_720p.mp4
```




# Miscellaneous

## Kafka producer from docker

```bash

pip install kafka-python-ng
cd /workspace/kafka
python producer_test.py
```

```bash
docker run -it confluentinc/cp-kafka:7.2.1 bash
kafka-console-producer --bootstrap-server 192.168.1.51:9092 -topic test {"test"}
```

## Install kafka in docker container ( No need for provided docker image )
```bash
git clone https://github.com/confluentinc/librdkafka.git
cd librdkafka
git checkout tags/v2.2.0
./configure --enable-ssl
make
make install
cp /usr/local/lib/librdkafka* /opt/nvidia/deepstream/deepstream/lib/
ldconfig


apt-get install libglib2.0 libglib2.0-dev libjansson4 libjansson-dev
```

## Structure of nvmsgconv configs


```dstest4_msgconv_config.txt
[sensor0]
enable=1
type=Camera
id=CAMERA_ID
location=45.293701447;-75.8303914499;48.1557479338
description="Entrance of Garage Right Lane"
coordinate=5.2;10.1;11.2

[place0]
enable=1
id=1
type=garage
name=XYZ
location=30.32;-40.55;100.0
coordinate=1.0;2.0;3.0
place-sub-field1=walsh
place-sub-field2=lane1
place-sub-field3=P2

[place1]
enable=1
id=1
type=garage
name=XYZ
location=28.47;47.46;1.53
coordinate=1.0;2.0;3.0
place-sub-field1="C-76-2"
place-sub-field2="LEV/EV/CP/ADA"
place-sub-field3=P2

[analytics0]
enable=1
id=XYZ
description="Vehicle Detection and License Plate Recognition"
source=OpenALR
version=1.0
```
custom json message 
https://forums.developer.nvidia.com/t/custom-json-message/176930
https://github.com/luchoaraya30/deepstream-custom-json-message

https://github.com/NVIDIA-AI-IOT/deepstream-retail-analytics/tree/main/nvmsgconv/deepstream_schema

sending images 
https://forums.developer.nvidia.com/t/nvmsgconv-custom-payload-to-send-image-and-its-metadata-through-message-broker/69851 
