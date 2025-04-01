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























# Extra 


```bash

curl -XPOST 'http://localhost:9000/api/v1/stream/add' -d '{
  "key": "sensor",
  "value": {
     "camera_id": "uniqueSensorID1",
     "camera_name": "front_door",
     "camera_url": "file:///opt/nvidia/deepstream/deepstream/samples/streams/sample_1080p_h264.mp4",
     "change": "camera_add",
     "metadata": {
         "resolution": "1920 x1080",
         "codec": "h264",
         "framerate": 30
     }
 },
 "headers": {
     "source": "vst",
     "created_at": "2021-06-01T14:34:13.417Z"
 }
}'




curl -XPOST 'http://localhost:9000/api/v1/stream/add' -d '{
  "key": "sensor",
  "value": {
     "camera_id": "rtspSensorID2",
     "camera_name": "rtsp_stream",
     "camera_url": "rtsp://192.168.1.66:8554/live/da890b5e-20ad-4f07-89f1-8de68ad017bf",
     "change": "camera_add",
     "metadata": {
         "resolution": "1920 x1080",
         "codec": "h264",
         "framerate": 30
     }
 },
 "headers": {
     "source": "vst",
     "created_at": "2021-06-01T14:34:13.417Z"
 }
}'

curl -XPOST 'http://localhost:9000/api/v1/stream/add' -d '{
  "key": "sensor",
  "value": {
     "camera_id": "rtspSensorID2",
     "camera_name": "rtsp_stream",
     "camera_url": "rtsp://192.168.1.66:8554/live/da890b5e-20ad-4f07-89f1-8de68ad017bf",
     "change": "camera_remove",
     "metadata": {
         "resolution": "1920 x1080",
         "codec": "h264",
         "framerate": 30
     }
 },
 "headers": {
     "source": "vst",
     "created_at": "2021-06-01T14:34:13.417Z"
 }
}'
```








```txt
[source0]
enable=1
#Type - 1=CameraV4L2 2=URI 3=MultiURI 4=RTSP
type=4
uri=rtsp://foo.com/stream1.mp4
num-sources=1
gpu-id=0
nvbuf-memory-type=0
# smart record specific fields, valid only for source type=4
# 0 = disable, 1 = through cloud events, 2 = through cloud + local events
#smart-record=1
# 0 = mp4, 1 = mkv
#smart-rec-container=0
#smart-rec-file-prefix
#smart-rec-dir-path
# smart record cache size in seconds
#smart-rec-cache
# default duration of recording in seconds.
#smart-rec-default-duration
# duration of recording in seconds.
# this will override default value.
#smart-rec-duration
# seconds before the current time to start recording.
#smart-rec-start-time
# value in seconds to dump video stream.
#smart-rec-interval
```

/opt/nvidia/deepstream/deepstream-7.0/sources/apps/apps-common/src/deepstream-yaml/deepstream_source_yaml.cpp


parse_source_yaml (NvDsSourceConfig *config, std::vector<std::string> headers,
    std::vector<std::string> source_values,  gchar *cfg_file_path)

paramKey == "smart-record"
config->smart_record = std::stoul(source_values[i]);

paramKey == "smart-rec-dir-path"
config->dir_path

else if (paramKey == "smart-rec-cache") {
      config->smart_rec_cache_size =
        std::stoul(source_values[i]); }

duration
default duration
interval



/opt/nvidia/deepstream/deepstream-7.0/sources/apps/apps-common/src/deepstream_source_bin.c

create_rtsp_src_bin (NvDsSourceConfig * config, NvDsSrcBin * bin)
if (config->smart_record)


NvDsSRContext *ctx = NULL;
NvDsSRInitParams
NvDsSRCreate (&ctx, &params)

params.callback = smart_record_callback;
smart_record_callback (NvDsSRRecordingInfo * info, gpointer userData)  -> just logs recording events



regular interval smart recording 
static gboolean smart_record_event_generator (gpointer data)

 NvDsSRStart (ctx, &sessId, startTime, duration, NULL);
 NvDsSRStop (ctx, 0);


 NvDsSRContext  -> recordbin



 NvDsSRStatus NvDsSRCreate (NvDsSRContext **ctx, NvDsSRInitParams *params);
 NvDsSRStatus NvDsSRStart (NvDsSRContext *ctx, NvDsSRSessionId *sessionId,
                          guint startTime, guint duration, gpointer userData);
NvDsSRStatus NvDsSRStop (NvDsSRContext *ctx, NvDsSRSessionId sessionId);

Recording = sessionId

NvDsSrcBin->recordCtx 

      g_timeout_add (bin->config->smart_rec_interval * 1000,
          smart_record_event_generator, bin);



pipeline

get GstElement -> represent element as BIN



NvDsSourceConfig
  guint smart_rec_cache_size;
  guint smart_rec_container;
  guint smart_rec_def_duration;
  guint smart_rec_duration;
  guint smart_rec_start_time;
  guint smart_rec_interval;


    NvDsSRInitParams params = { 0 };
    params.containerType = (NvDsSRContainerType) config->smart_rec_container;
    if (config->file_prefix)
      params.fileNamePrefix =
          g_strdup_printf ("%s_%d", config->file_prefix, config->camera_id);
    params.dirpath = config->dir_path;
    params.cacheSize = config->smart_rec_cache_size;
    params.defaultDuration = config->smart_rec_def_duration;
    params.callback = smart_record_callback;


```c++
    NvDsSRContext *ctx = NULL; // recordbin, recordCtx    gst-nvdssr.h /opt/nvidia/deepstream/deepstream-7.0/sources/includes/gst-nvdssr.h
    NvDsSRInitParams params = { 0 }; 
    NvDsSRCreate (&ctx, &params) != NVDSSR_STATUS_OK
    // NvDsSrcBin * bin 
    // bin->bin  (GstElement)  - pipeline 
    gst_bin_add (GST_BIN (bin->bin), ctx->recordbin); //  GstElement *recordbin;
    bin->recordCtx = (gpointer) ctx;

    create_rtsp_src_bin

      if (ctx)
    link_element_to_tee_src_pad (bin->tee_rtsp_pre_decode, ctx->recordbin);

    //before decode    src_elem (rtspsrc) -> tee_rtsp_pre_decode -> (parser) -> recordbin



    nvurisrcbin -> already supports smart-record

    smart-record=0 // disabled
    smart-record=1 // cloud
    smart-record=2 // cloud+local
    // source type must be rtsp
    smart-rec-dir-path=
    smart-rec-file-prefix=
    smart-rec-video-cache=10 // seconds
    smart-rec-container=0  // mp4
    smart-rec-mode=0 // only video
    smart-rec-default-duration=10 // if no stop event


// where is kafka config ? 
    // start stop recordings with event
    ctx = nvurisrcbin? 

    ctx = NvDsSRContext? // how to get from nvuriscrbin?

    //NvDsSRSessionId *sessionId
    // startTime int - seconds before current time
    // duration int - duration in seconds
    // userData?

 NvDsSRStart (ctx, &sessId, startTime, duration, NULL);
 NvDsSRStop (ctx, 0);

```