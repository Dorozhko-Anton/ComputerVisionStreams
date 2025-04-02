https://docs.nvidia.com/metropolis/deepstream/dev-guide/text/DS_ref_app_test5.html


ds -> messages -> backend
backend -> messages -> ds

event-based recording

OTA model update
add Sensors with REST API


# SmartRecording 

gst-nvdssr.h


## cloud message 

smart-record=1
[message-consumerX] 

```json
{
  command: string   // <start-recording / stop-recording>
  start: string     // "2020-05-18T20:02:00.051Z"
  end: string       // "2020-05-18T20:02:02.851Z",
  sensor: {
  id: string
  }
 }
```

## local events

smart-record=2
by default triggers start / stop events every ten seconds


# Run test app 

```bash
docker login nvcr.io
docker build -t deepstream:7.0 .
```

```bash
# Step to run the docker
export DISPLAY=:0
xhost +
docker run -it --rm --net=host --gpus all -e DISPLAY=$DISPLAY --device /dev/snd -v /tmp/.X11-unix/:/tmp/.X11-unix -v $PWD:/workspace deepstream:7.0 bash
```



nvmultiurisrcbin

```bash
cd /opt/nvidia/deepstream/deepstream/sources/apps/sample_apps/deepstream-test5/configs
deepstream-test5-app -c test5_config_file_nvmultiurisrcbin_src_list_attr_all.txt
```

sample pipeline 

```
cd /workspace/multiurisrcbin/
mkdir build
cd build && cmake .. && make

./multiutisrc_sample
```

# ADD/REMOVE STREAMS 

```bash
curl -XPOST 'http://localhost:9000/api/v1/stream/add' -d '{
  "key": "sensor",
  "value": {
     "camera_id": "rtspSensorID2",
     "camera_name": "rtsp_stream",
     "camera_url": "rtsp://192.168.1.66:8554/live/da890b5e-20ad-4f07-89f1-8de68ad017bf",
     "change": "camera_add",
  }}'


curl -XPOST 'http://localhost:9000/api/v1/stream/add' -d '{
  "key": "sensor",
  "value": {
     "camera_id": "rtspSensorID2",
     "camera_name": "rtsp_stream",
     "camera_url": "rtsp://192.168.1.66:8554/live/da890b5e-20ad-4f07-89f1-8de68ad017bf",
     "change": "camera_remove",
  }}'

curl -XPOST 'http://localhost:9000/api/v1/stream/remove' -d '{
  "key": "sensor",
  "value": {
     "camera_id": "rtspSensorID2",
     "camera_name": "rtsp_stream",
     "camera_url": "rtsp://192.168.1.66:8554/live/da890b5e-20ad-4f07-89f1-8de68ad017bf",
     "change": "camera_add",
  }}'


curl -XPOST 'http://localhost:9000/api/v1/stream/add' -d '{
  "key": "sensor",
  "value": {
     "camera_id": "rtspSensorID2",
     "camera_name": "rtsp_stream",
     "camera_url": "rtsp://192.168.1.66:8554/live/da890b5e-20ad-4f07-89f1-8de68ad017bf",
     "change": "camera_remove",
  }}'

curl -XPOST 'http://localhost:9000/api/v1/stream/remove' -d '{
  "key": "sensor",
  "value": {
     "camera_id": "rtspSensorID2",
     "camera_name": "rtsp_stream",
     "camera_url": "rtsp://192.168.1.66:8554/live/da890b5e-20ad-4f07-89f1-8de68ad017bf",
     "change": "camera_remove",
  }}'

```



# Links 

https://docs.nvidia.com/metropolis/deepstream-nvaie30/dev-guide/text/DS_plugin_gst-nvmultiurisrcbin.html 
https://docs.nvidia.com/metropolis/deepstream/dev-guide/text/DS_plugin_gst-nvurisrcbin.html 
https://docs.nvidia.com/metropolis/deepstream/dev-guide/text/DS_plugin_gst-nvmultiurisrcbin.html 
https://docs.nvidia.com/metropolis/deepstream/dev-guide/text/DS_RestServer.html#nvds-rest-server 
https://github.com/civetweb/civetweb/tree/master 
https://github.com/CrowCpp/Crow 







