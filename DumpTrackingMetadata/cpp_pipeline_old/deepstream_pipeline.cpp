#include <gst/gst.h>
#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "gstnvdsmeta.h"

#include <fstream>
#include <nlohmann/json.hpp>
#include <filesystem>

// for convenience
using json = nlohmann::json;


struct AppCtx {
  std::string trackoutput;
};

/**
 * Function to dump bounding box data in kitti format. For this to work,
 * property "gie-kitti-output-dir" must be set in configuration file.
 * Data of different sources and frames is dumped in separate file.
 */
//  #Values    Name      Description
// ----------------------------------------------------------------------------
//    1    type         Describes the type of object: 'Car', 'Van', 'Truck',
//                      'Pedestrian', 'Person_sitting', 'Cyclist', 'Tram',
//                      'Misc' or 'DontCare'
//    1    truncated    Float from 0 (non-truncated) to 1 (truncated), where
//                      truncated refers to the object leaving image boundaries
//    1    occluded     Integer (0,1,2,3) indicating occlusion state:
//                      0 = fully visible, 1 = partly occluded
//                      2 = largely occluded, 3 = unknown
//    1    alpha        Observation angle of object, ranging [-pi..pi]
//    4    bbox         2D bounding box of object in the image (0-based index):
//                      contains left, top, right, bottom pixel coordinates
//    3    dimensions   3D object dimensions: height, width, length (in meters)
//    3    location     3D object location x,y,z in camera coordinates (in meters)
//    1    rotation_y   Rotation ry around Y-axis in camera coordinates [-pi..pi]
//    1    score        Only for results: Float, indicating confidence in
//                      detection, needed for p/r curves, higher is better.

static void
write_kitti_output (std::string bbox_dir_path, int stream_index, NvDsBatchMeta * batch_meta)
{
  gchar bbox_file[1024] = { 0 };
  FILE *bbox_params_dump_file = NULL;
  if (bbox_dir_path.empty())
    return;

  struct stat st = {0};
  if (stat(bbox_dir_path.c_str(), &st) == -1) {
      mkdir(bbox_dir_path.c_str(), 0777);
  }

  for (NvDsMetaList * l_frame = batch_meta->frame_meta_list; l_frame != NULL;
      l_frame = l_frame->next) {
    NvDsFrameMeta *frame_meta = (NvDsFrameMeta *) l_frame->data;
    guint stream_id = frame_meta->pad_index;
    g_snprintf (bbox_file, sizeof (bbox_file) - 1,
        "%s/%02u_%03u_%06lu.txt", bbox_dir_path.c_str(),
        stream_index, stream_id, (gulong) frame_meta->frame_num);

    bbox_params_dump_file = fopen (bbox_file, "w");
    if (!bbox_params_dump_file)
      continue;

    for (NvDsMetaList * l_obj = frame_meta->obj_meta_list; l_obj != NULL;
        l_obj = l_obj->next) {
      NvDsObjectMeta *obj = (NvDsObjectMeta *) l_obj->data;
      float left = obj->rect_params.left;
      float top = obj->rect_params.top;
      float right = left + obj->rect_params.width;
      float bottom = top + obj->rect_params.height;
      // Here confidence stores detection confidence, since dump gie output
      // is before tracker plugin
      float confidence = obj->confidence;
      fprintf (bbox_params_dump_file,
          "%s 0.0 0 0.0 %f %f %f %f 0.0 0.0 0.0 0.0 0.0 0.0 0.0 %f\n",
          obj->obj_label, left, top, right, bottom, confidence);
    }
    fclose (bbox_params_dump_file);
  }
}

// /**
//  * Function to dump bounding box data in kitti format with tracking ID added.
//  * For this to work, property "kitti-track-output-dir" must be set in configuration file.
//  * Data of different sources and frames is dumped in separate file.
//  */
static void
write_kitti_track_output (std::string kitti_track_dir_path, int stream_index, NvDsBatchMeta * batch_meta)
{
  gchar bbox_file[1024] = { 0 };
  FILE *bbox_params_dump_file = NULL;

  if (kitti_track_dir_path.empty())
    return;

  struct stat st = {0};
  if (stat(kitti_track_dir_path.c_str(), &st) == -1) {
      mkdir(kitti_track_dir_path.c_str(), 0777);
  }

  for (NvDsMetaList * l_frame = batch_meta->frame_meta_list; l_frame != NULL;
      l_frame = l_frame->next) {
    NvDsFrameMeta *frame_meta = (NvDsFrameMeta *) l_frame->data;
    guint stream_id = frame_meta->pad_index;
    g_snprintf (bbox_file, sizeof (bbox_file) - 1,
        "%s/%02u_%03u_%06lu.txt", kitti_track_dir_path.c_str(),
        stream_index, stream_id, (gulong) frame_meta->frame_num);
    bbox_params_dump_file = fopen (bbox_file, "w");
    if (!bbox_params_dump_file)
      continue;

    for (NvDsMetaList * l_obj = frame_meta->obj_meta_list; l_obj != NULL;
        l_obj = l_obj->next) {
      NvDsObjectMeta *obj = (NvDsObjectMeta *) l_obj->data;
      float left = obj->tracker_bbox_info.org_bbox_coords.left;
      float top = obj->tracker_bbox_info.org_bbox_coords.top;
      float right = left + obj->tracker_bbox_info.org_bbox_coords.width;
      float bottom = top + obj->tracker_bbox_info.org_bbox_coords.height;
      // Here confidence stores tracker confidence value for tracker output
      float confidence = obj->tracker_confidence;
      guint64 id = obj->object_id;
      fprintf (bbox_params_dump_file,
        "%s %lu 0.0 0 0.0 %f %f %f %f 0.0 0.0 0.0 0.0 0.0 0.0 0.0 %f\n",
        obj->obj_label, id, left, top, right, bottom, confidence);
    
    }
    fclose (bbox_params_dump_file);
  }
}


/**
 * Buffer probe function to get the results of primary infer.
 * Here it demonstrates the use by dumping bounding box coordinates in
 * kitti format.
 */
static GstPadProbeReturn
gie_primary_processing_done_buf_prob (GstPad * pad, GstPadProbeInfo * info,
    gpointer u_data)
{
  GstBuffer *buf = (GstBuffer *) info->data;
  NvDsBatchMeta *batch_meta = gst_buffer_get_nvds_batch_meta (buf);
  if (!batch_meta) {
    return GST_PAD_PROBE_OK;
  }
  std::string bbox_dir_path = "kitti_output"; 
  int stream_index = 1;
  write_kitti_output (bbox_dir_path, stream_index, batch_meta);
  return GST_PAD_PROBE_OK;
}

/**
 * Buffer probe function after tracker.
 */
static GstPadProbeReturn
tracker_done_buf_prob (GstPad * pad, GstPadProbeInfo * info, gpointer u_data)
{
  GstBuffer *buf = (GstBuffer *) info->data;
  AppCtx * appCtx = (AppCtx *) u_data;
  NvDsBatchMeta *batch_meta = gst_buffer_get_nvds_batch_meta (buf);
  if (!batch_meta) {
    return GST_PAD_PROBE_OK;
  }

  /*
   * Output KITTI labels with tracking ID if configured to do so.
   */
  std::string bbox_dir_path = appCtx->trackoutput;//"kitti_track_output"; 
  int stream_index = 1;
  write_kitti_track_output (bbox_dir_path, stream_index, batch_meta);
  //write_kitti_past_track_output (appCtx, batch_meta);
  // write_reid_track_output(appCtx, batch_meta);
  // write_terminated_track_output(appCtx, batch_meta);
  // write_shadow_track_output(appCtx, batch_meta);
  // if (appCtx->bbox_generated_post_analytics_cb) {
  //   appCtx->bbox_generated_post_analytics_cb (appCtx, buf, batch_meta, index);
  // }
  return GST_PAD_PROBE_OK;
}

static gboolean
bus_call(GstBus *bus, GstMessage *msg, gpointer data)
{
  GMainLoop *loop = (GMainLoop *)data;
  switch (GST_MESSAGE_TYPE(msg))
  {
  case GST_MESSAGE_EOS:
    g_print("End of stream\n");
    g_main_loop_quit(loop);
    break;
  case GST_MESSAGE_ERROR:
  {
    gchar *debug;
    GError *error;
    gst_message_parse_error(msg, &error, &debug);
    g_printerr("ERROR from element %s: %s\n",
               GST_OBJECT_NAME(msg->src), error->message);
    if (debug)
      g_printerr("Error details: %s\n", debug);
    g_free(debug);
    g_error_free(error);
    g_main_loop_quit(loop);
    break;
  }
  default:
    break;
  }
  return TRUE;
}


int main(int argc, char *argv[])
{
  /* Standard GStreamer initialization */
  gst_init(&argc, &argv);
  GMainLoop *loop = g_main_loop_new(NULL, FALSE);
  GstElement *pipeline = NULL;
  
  AppCtx * appCtx = new AppCtx();

  std::ifstream ifs(argv[1]);
  json j_config = json::parse(ifs);
  std::cout << "Config:" << '\n'
              << j_config.dump(-1);

  std::string output_dir = j_config.at("output_dir").get<std::string>();
  std::filesystem::path tracking_output = std::filesystem::path(output_dir);
  std::filesystem::create_directories(tracking_output);
  appCtx->trackoutput = output_dir;


  std::string pipeline_description = R"foo(
      nvurisrcbin 
          name=src 
          uri=)foo" + j_config.at("uri").get<std::string>() +
      R"foo(
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
      ! nvdsosd name=osd
      ! nveglglessink name=sink
      )foo";

  std::cout << "PIPELINE \n" << pipeline_description << std::endl;


  pipeline = gst_parse_launch(pipeline_description.c_str(), NULL);
   /* we add a message handler */
  GstBus *bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline));
  int bus_watch_id = gst_bus_add_watch(bus, bus_call, loop);
  gst_object_unref(bus);

  // GstElement *infer = gst_bin_get_by_name(GST_BIN(pipeline), "infer");
  // GstPad *infer_src_pad = gst_element_get_static_pad(infer, "src");
  // gst_pad_add_probe(infer_src_pad, GST_PAD_PROBE_TYPE_BUFFER,
  //                   gie_primary_processing_done_buf_prob, NULL, NULL);
  
  GstElement *tracker = gst_bin_get_by_name(GST_BIN(pipeline), "tracker");
  GstPad *tracker_src_pad = gst_element_get_static_pad(tracker, "src");
  gst_pad_add_probe(tracker_src_pad, GST_PAD_PROBE_TYPE_BUFFER,
                    tracker_done_buf_prob, appCtx, NULL);
  


  /* Set the pipeline to "playing" state */
  gst_element_set_state(pipeline, GST_STATE_PLAYING);
  /* Iterate */
  g_main_loop_run(loop);
  /* Out of the main loop, clean up nicely */
  gst_element_set_state(pipeline, GST_STATE_NULL);
  gst_object_unref(GST_OBJECT(pipeline));
  g_source_remove(bus_watch_id);
  g_main_loop_unref(loop);
  return 0;
}