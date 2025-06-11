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

  // gst-launch-1.0 filesrc location=/opt/nvidia/deepstream/deepstream/samples/streams/sample_720p.h264 ! h264parse ! nvv4l2decoder ! \
  //   recordplugin name=rec ! nvvideoconvert ! nveglglessink

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
      ! recordplugin name=rawrec
      ! nvinfer 
        name=infer 
        config-file-path=/opt/nvidia/deepstream/deepstream/samples/configs/deepstream-app/config_infer_primary.yml
      ! nvdsosd name=osd
      ! recordplugin name=rec
      ! nveglglessink name=sink
      )foo";

  std::cout << "PIPELINE \n"
            << pipeline_description << std::endl;

  pipeline = gst_parse_launch(pipeline_description.c_str(), NULL);

  GstElement *rec = gst_bin_get_by_name(GST_BIN(pipeline), "rec");
  GstElement *rawrec = gst_bin_get_by_name(GST_BIN(pipeline), "rawrec");
  std::thread record_thread([rec, rawrec]()
                            {
                              g_print("Wait before sending record signal...\n");
                              // Wait for 5 seconds
                              std::this_thread::sleep_for(std::chrono::seconds(10));
                              // Start recording
                              g_print("Sending record-start signal...\n");
                              g_signal_emit_by_name(rec, "record-start", NULL);
                              g_signal_emit_by_name(rawrec, "record-start", NULL);
                              // Wait for 5 seconds
                              std::this_thread::sleep_for(std::chrono::seconds(5));
                              // Stop recording
                              g_print("Sending record-stop signal...\n");
                              g_signal_emit_by_name(rec, "record-stop", NULL);
                              g_signal_emit_by_name(rawrec, "record-stop", NULL);

                              std::this_thread::sleep_for(std::chrono::seconds(5));
                              // Start recording
                              g_print("Sending 2nd record-start signal...\n");
                              g_signal_emit_by_name(rec, "record-start", NULL);
                              // Wait for 5 seconds
                              std::this_thread::sleep_for(std::chrono::seconds(5));
                              // Stop recording
                              g_print("Sending record-stop signal...\n");
                              g_signal_emit_by_name(rec, "record-stop", NULL);
                            });

  // Detach or join the thread depending on use case
  record_thread.detach(); // Or use .join() if you want to wait

  /* Set the pipeline to "playing" state */
  gst_element_set_state(pipeline, GST_STATE_PLAYING);

  std::thread thread([pipeline]()
                     {
  // Wait for 5 seconds
    std::this_thread::sleep_for(std::chrono::seconds(5));
      GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN(pipeline), GST_DEBUG_GRAPH_SHOW_ALL, "pipeline"); });
  thread.detach();
  /* Iterate */
  g_main_loop_run(loop);
  /* Out of the main loop, clean up nicely */
  gst_element_set_state(pipeline, GST_STATE_NULL);
  gst_object_unref(GST_OBJECT(pipeline));
  g_main_loop_unref(loop);
  return 0;
}