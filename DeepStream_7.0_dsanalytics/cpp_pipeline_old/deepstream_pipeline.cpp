#include <gst/gst.h>
#include <iostream>
#include <string>
#include <sstream>
#include "gstnvdsmeta.h"
// nvdsanalytics 
#include "nvds_analytics_meta.h"


static GstPadProbeReturn
nvdsanalytics_src_pad_buffer_probe (GstPad * pad, GstPadProbeInfo * info,
    gpointer u_data)
{
    GstBuffer *buf = (GstBuffer *) info->data;
    guint num_rects = 0;
    NvDsObjectMeta *obj_meta = NULL;
    guint vehicle_count = 0;
    guint person_count = 0;
    NvDsMetaList * l_frame = NULL;
    NvDsMetaList * l_obj = NULL;

    NvDsBatchMeta *batch_meta = gst_buffer_get_nvds_batch_meta (buf);

    for (l_frame = batch_meta->frame_meta_list; l_frame != NULL;
      l_frame = l_frame->next) {
        NvDsFrameMeta *frame_meta = (NvDsFrameMeta *) (l_frame->data);
        std::stringstream out_string;
        vehicle_count = 0;
        num_rects = 0;
        person_count = 0;
        for (l_obj = frame_meta->obj_meta_list; l_obj != NULL;
                l_obj = l_obj->next) {
            obj_meta = (NvDsObjectMeta *) (l_obj->data);
            if (obj_meta->class_id == 0) {
                vehicle_count++;
                num_rects++;
            }
            if (obj_meta->class_id == 2) {
                person_count++;
                num_rects++;
            }


            // Access attached user meta for each object
            for (NvDsMetaList *l_user_meta = obj_meta->obj_user_meta_list; l_user_meta != NULL;
                    l_user_meta = l_user_meta->next) {

                      
                NvDsUserMeta *user_meta = (NvDsUserMeta *) (l_user_meta->data);
                if(user_meta->base_meta.meta_type == NVDS_USER_OBJ_META_NVDSANALYTICS)
                {   
                  std::stringstream object_out_string;
                  object_out_string << "Object " << obj_meta->object_id << "\n";

                    NvDsAnalyticsObjInfo * user_meta_data = (NvDsAnalyticsObjInfo *)user_meta->user_meta_data;
                    if (user_meta_data->dirStatus.length()){
                      object_out_string << "dirStatus: " << user_meta_data->dirStatus.c_str() << "\n";
                    }

                    for (std::string name : user_meta_data->roiStatus){
                       object_out_string << " roiStatus: " << name << "\n";
                  }
      
                  for (std::string name : user_meta_data->ocStatus){
                    object_out_string << " ocStatus: " << name << "\n";
                  }
                  for (std::string name : user_meta_data->lcStatus){
                    object_out_string << " lcStatus: " << name << "\n";
                  }

                  g_print("%s\n", object_out_string.str().c_str());
                }
            }
            
        }

        /* Iterate user metadata in frames to search analytics metadata */
        for (NvDsMetaList * l_user = frame_meta->frame_user_meta_list;
                l_user != NULL; l_user = l_user->next) {
            NvDsUserMeta *user_meta = (NvDsUserMeta *) l_user->data;
            if (user_meta->base_meta.meta_type != NVDS_USER_FRAME_META_NVDSANALYTICS)
                continue;

            /* convert to  metadata */
            NvDsAnalyticsFrameMeta *meta =
                (NvDsAnalyticsFrameMeta *) user_meta->user_meta_data;
            /* Get the labels from nvdsanalytics config file */
            for (std::pair<std::string, uint32_t> status : meta->objInROIcnt){
                out_string << "Objs in ROI: ";
                out_string << status.first;
                out_string << " = ";
                out_string << status.second;
                out_string << "\n";
            }

            for (std::pair<std::string, uint32_t> status : meta->objLCCumCnt){
                out_string << " LineCrossing Cumulative: ";
                out_string << status.first;
                out_string << " = ";
                out_string << status.second;
                out_string << "\n";
            }
            for (std::pair<std::string, uint32_t> status : meta->objLCCurrCnt){
                out_string << " LineCrossing Current Frame: ";
                out_string << status.first;
                out_string << " = ";
                out_string << status.second;
                out_string << "\n";
            }
            for (std::pair<std::string, bool> status : meta->ocStatus){
                out_string << " Overcrowding status: ";
                out_string << status.first;
                out_string << " = ";
                out_string << status.second;
                out_string << "\n";
            }
        }
        g_print ("Frame Number = %d of Stream = %d \n Number of objects = %d \n"
                "Vehicle Count = %d \n Person Count = %d \n %s\n",
            frame_meta->frame_num, frame_meta->pad_index,
            num_rects, vehicle_count, person_count, out_string.str().c_str());
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
          file-loop=1
      ! mux.sink_0 
      
      nvstreammux 
        name=mux 
        batch-size=1 
        width=1280 
        height=720
      ! nvinfer 
        name=infer 
        config-file-path=/opt/nvidia/deepstream/deepstream/samples/configs/deepstream-app/config_infer_primary.yml
      ! nvtracker
        name=tracker
        ll-lib-file = /opt/nvidia/deepstream/deepstream/lib/libnvds_nvmultiobjecttracker.so 
        ll-config-file=/opt/nvidia/deepstream/deepstream/samples/configs/deepstream-app/config_tracker_NvDCF_perf.yml
        tracker-width=640 
        tracker-height=384
      ! nvdsanalytics 
        name=nvdsanalytics
        enable=1
        config-file=/workspace/cpp_pipeline_old/cfg_nvdsanalytics.txt
      ! nvdsosd name=osd
      ! nvdslogger fps-measurement-interval-sec=1
      ! nveglglessink name=sink sync=1
      )foo";

  std::cout << "PIPELINE \n" << pipeline_description << std::endl;

  pipeline = gst_parse_launch(pipeline_description.c_str(), NULL);

  GstElement *nvdsanalytics = gst_bin_get_by_name(GST_BIN(pipeline), "nvdsanalytics");
  GstPad *nvdsanalytics_pad = gst_element_get_static_pad (nvdsanalytics, "src");
  gst_pad_add_probe (nvdsanalytics_pad, GST_PAD_PROBE_TYPE_BUFFER,
    nvdsanalytics_src_pad_buffer_probe, NULL, NULL);
  

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