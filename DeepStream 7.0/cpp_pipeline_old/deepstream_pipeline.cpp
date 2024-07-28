#include <gst/gst.h>
#include <iostream>
#include <string>
#include "gstnvdsmeta.h"


static GstPadProbeReturn
count_probe(GstPad *pad, GstPadProbeInfo *info, gpointer u_data)
{
  auto vehicle_count = 0;
  NvDsBatchMeta *batch_meta =
      gst_buffer_get_nvds_batch_meta(GST_BUFFER(info->data));
  /* Iterate each frame metadata in batch */
  for (NvDsMetaList *l_frame = batch_meta->frame_meta_list; l_frame != NULL;
       l_frame = l_frame->next)
  {
    NvDsFrameMeta *frame_data = (NvDsFrameMeta *)l_frame->data;
    /* Iterate object metadata in frame */
    for (NvDsMetaList *l_obj = frame_data->obj_meta_list; l_obj != NULL;
         l_obj = l_obj->next)
    {
      NvDsObjectMeta *object_data = (NvDsObjectMeta *)l_obj->data;
      auto class_id = object_data->class_id;
      if (class_id == 0) {
          vehicle_count++;
      } 
    }
    std::cout << "Object Counter: " <<
            " Pad Idx = " << frame_data->pad_index <<
            " Frame Number = " << frame_data->frame_num <<
            " Vehicle Count = " << vehicle_count << std::endl;
  }
  return GST_PAD_PROBE_OK;
}


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
      ! mux.sink_0 
      
      nvstreammux 
        name=mux 
        batch-size=1 
        width=1280 
        height=720
      ! nvinfer 
        name=infer 
        config-file-path=/opt/nvidia/deepstream/deepstream/samples/configs/deepstream-app/config_infer_primary.yml
      ! nvdsosd name=osd
      ! nveglglessink name=sink
      )foo";

  std::cout << "PIPELINE \n" << pipeline_description << std::endl;

  pipeline = gst_parse_launch(pipeline_description.c_str(), NULL);

  GstElement *infer = gst_bin_get_by_name(GST_BIN(pipeline), "infer");
  GstPad *infer_src_pad = gst_element_get_static_pad(infer, "src");
  gst_pad_add_probe(infer_src_pad, GST_PAD_PROBE_TYPE_BUFFER,
                    count_probe, NULL, NULL);

  

  /* Set the pipeline to "playing" state */
  gst_element_set_state(pipeline, GST_STATE_PLAYING);
  /* Iterate */
  g_main_loop_run(loop);
  /* Out of the main loop, clean up nicely */
  gst_element_set_state(pipeline, GST_STATE_NULL);
  gst_object_unref(GST_OBJECT(pipeline));
  g_main_loop_unref(loop);
  return 0;
}