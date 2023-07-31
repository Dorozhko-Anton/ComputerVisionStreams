# Car detection pipeline
gst-launch-1.0 filesrc location=./streams/sample_1080p_h264.mp4 ! qtdemux ! h264parse ! nvv4l2decoder \
! m.sink_0 nvstreammux name=m batch-size=1 width=1920 height=1080 ! nvinfer config-file-path= configs/deepstream-app/config_infer_primary.txt \
batch-size=1 unique-id=1 ! nvtracker ll-lib-file=/opt/nvidia/deepstream/deepstream/lib/libnvds_nvmultiobjecttracker.so \
! nvinfer config-file-path= configs/deepstream-app/config_infer_secondary_carcolor.txt batch-size=16 unique-id=2 infer-on-gie-id=1 infer-on-class-ids=0 \
! nvmultistreamtiler rows=1 columns=1 width=1280 height=720 ! nvvideoconvert ! nvdsosd ! nvvideoconvert ! videoconvert ! xvimagesink

# Get URL stream with youtube-dl
youtube-dl --format "best[ext=mp4]" --get-url https://www.youtube.com/watch?v=z6BNMoj9Pyo

# Check that stream is visible
gst-launch-1.0 souphttpsrc is-live=true location="$(youtube-dl --format "best[ext=mp4]" --get-url https://www.youtube.com/watch?v=z6BNMoj9Pyo)" ! decodebin ! videoconvert ! nvvideoconvert ! xvimagesink

gst-launch-1.0 souphttpsrc is-live=true location="$(youtube-dl --format "best[ext=mp4]" --get-url https://www.youtube.com/watch?v=VjvqGR3wsIs)" ! decodebin ! videoconvert ! nvvideoconvert ! xvimagesink



# Run out pipeline with a chosen stream by changing filesrc to souphttpsrc

gst-launch-1.0 souphttpsrc is-live=true location="$(youtube-dl --format "best[ext=mp4]" --get-url https://www.youtube.com/watch?v=z6BNMoj9Pyo)" ! decodebin \
! m.sink_0 nvstreammux name=m batch-size=1 width=1920 height=1080 ! nvinfer config-file-path= configs/deepstream-app/config_infer_primary.txt \
batch-size=1 unique-id=1 ! nvtracker ll-lib-file=/opt/nvidia/deepstream/deepstream/lib/libnvds_nvmultiobjecttracker.so \
! nvinfer config-file-path= configs/deepstream-app/config_infer_secondary_carcolor.txt batch-size=16 unique-id=2 infer-on-gie-id=1 infer-on-class-ids=0 \
! nvmultistreamtiler rows=1 columns=1 width=1280 height=720 ! nvvideoconvert ! nvdsosd ! nvvideoconvert ! videoconvert ! xvimagesink

gst-launch-1.0 souphttpsrc is-live=true location="$(youtube-dl --format "best[ext=mp4]" --get-url https://www.youtube.com/watch?v=VjvqGR3wsIs)" ! decodebin \
! m.sink_0 nvstreammux name=m batch-size=1 width=1920 height=1080 ! nvinfer config-file-path= configs/deepstream-app/config_infer_primary.txt \
batch-size=1 unique-id=1 ! nvtracker ll-lib-file=/opt/nvidia/deepstream/deepstream/lib/libnvds_nvmultiobjecttracker.so \
! nvinfer config-file-path= configs/deepstream-app/config_infer_secondary_carcolor.txt batch-size=16 unique-id=2 infer-on-gie-id=1 infer-on-class-ids=0 \
! nvmultistreamtiler rows=1 columns=1 width=1280 height=720 ! nvvideoconvert ! nvdsosd ! nvvideoconvert ! videoconvert ! xvimagesink