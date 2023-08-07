# test video
gst-launch-1.0 videotestsrc ! videoconvert ! autovideosink

# set parameters: scale test video 
gst-launch-1.0 videotestsrc pattern=11 ! videoconvert ! videoscale ! video/x-raw,width=1920,height=1080 ! autovideosink

# How to see parameters of an element
gst-inspect-1.0 videotestsrc
# or go to documentation of an element https://gstreamer.freedesktop.org/documentation/videotestsrc/index.html?gi-language=c
# to find documentation for an element google : "gstreamer <ELEMENT_NAME>"


# Read from camera
# share camera device to docker with --device /dev/video0
# video caps
gst-launch-1.0 v4l2src ! video/x-raw,width=320,height=240 ! videoconvert ! autovideosink
# Filter example 
gst-launch-1.0 videotestsrc ! videoconvert ! edgetv ! videoconvert ! autovideosink

# Read from online video
# Read from rtsp
gst-launch-1.0 uridecodebin uri=https://gstreamer.freedesktop.org/data/media/sintel_trailer-480p.webm ! queue ! videoscale ! video/x-raw,width=320,height=200 ! videoconvert ! autovideosink

# Play video file
gst-launch-1.0 playbin uri=https://gstreamer.freedesktop.org/data/media/sintel_trailer-480p.webm


# Debug graph of a pipeline 
GST_DEBUG_DUMP_DOT_DIR=./ gst-launch-1.0 videotestsrc ! videoconvert ! autovideosink
dot -Tsvg filename.dot -o outfile.svg

GST_DEBUG_DUMP_DOT_DIR=./ gst-launch-1.0 uridecodebin uri=https://gstreamer.freedesktop.org/data/media/sintel_trailer-480p.webm ! queue ! videoscale ! video/x-raw,width=320,height=200 ! videoconvert ! autovideosink









# Appsrc
gst-launch-1.0 testvideosrc ! videoconvert ! appsink name=output


# openCV support 
# install opencv with Gstreamer support https://github.com/opencv/opencv-python/issues/530
# https://answers.opencv.org/question/202017/how-to-use-gstreamer-pipeline-in-opencv/
# Usage:
import cv2
import matplotlib.pylab as plt
print(cv2.getBuildInformation())
vc = cv2.VideoCapture('videotestsrc ! video/x-raw,framerate=20/1 ! videoscale ! videoconvert ! appsink', cv2.CAP_GSTREAMER)
vc.isOpened()
status, image = vc.read()

plt.imshow(image[:,:,::-1])

# receive and send
# https://answers.opencv.org/question/202017/how-to-use-gstreamer-pipeline-in-opencv/



