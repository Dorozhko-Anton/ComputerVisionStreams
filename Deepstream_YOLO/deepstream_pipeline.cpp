#include <gst/gst.h>
#include <iostream>
#include <string>
#include "gstnvdsmeta.h"
#include <thread>
#include <chrono>

int main(int argc, char *argv[])
{
  /* Standard GStreamer initialization */
  gst_init(&argc, &argv);
  GMainLoop *loop = g_main_loop_new(NULL, FALSE);
  GstElement *pipeline = NULL;

  std::string pipeline_description = R"foo(
      nvurisrcbin 
          name=src 
          uri=file:///opt/nvidia/deepstream/deepstream/samples/streams/sample_720p.mp4
          file-loop=1
      ! mux.sink_0 
      
      nvstreammux 
        name=mux 
        batch-size=1 
        width=1280 
        height=720
      ! nvinfer 
        name=infer 
        config-file-path=/workspace/build/DeepStream-Yolo/config_infer_primary_yolo11.txt
      ! nvdslogger
      ! nvdsosd name=osd
      ! nveglglessink name=sink sync=true
      )foo";
// ! nveglglessink name=sink sync=true
      
  std::cout << "PIPELINE \n"
            << pipeline_description << std::endl;

  pipeline = gst_parse_launch(pipeline_description.c_str(), NULL);

  /* Set the pipeline to "playing" state */
  gst_element_set_state(pipeline, GST_STATE_PLAYING);

  // std::thread thread([pipeline]()
  //                    {
  // // Wait for 5 seconds
  //   std::this_thread::sleep_for(std::chrono::seconds(5));
  //     GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN(pipeline), GST_DEBUG_GRAPH_SHOW_ALL, "pipeline"); });
  // thread.detach();
  /* Iterate */
  g_main_loop_run(loop);
  /* Out of the main loop, clean up nicely */
  gst_element_set_state(pipeline, GST_STATE_NULL);
  gst_object_unref(GST_OBJECT(pipeline));
  g_main_loop_unref(loop);
  return 0;
}