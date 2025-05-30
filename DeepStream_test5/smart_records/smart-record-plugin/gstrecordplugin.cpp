#include "gstrecordplugin.h"
#include <gst/gst.h>
#include <gst/video/video.h>
#include <gst/app/gstappsink.h>
#include <gst/app/gstappsrc.h>
#include <iostream>
#include <fstream>
#include <mutex>

GST_DEBUG_CATEGORY_STATIC(gst_record_debug);
#define GST_CAT_DEFAULT gst_record_debug

struct _GstRecord {
    GstBaseTransform parent;
    gboolean is_recording;
    GstElement *pipeline;
    GstElement *appsrc;
    std::mutex record_mutex;
};

G_DEFINE_TYPE(GstRecord, gst_record, GST_TYPE_BASE_TRANSFORM)

static
    GstStaticPadTemplate
    gst_recordplugin_src_template = GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS_ANY);

static
    GstStaticPadTemplate
    gst_recordplugin_sink_template = GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS_ANY);


enum {
    SIGNAL_START_RECORD,
    SIGNAL_STOP_RECORD,
    LAST_SIGNAL
};

static guint gst_record_signals[LAST_SIGNAL] = {0};

static GstFlowReturn gst_record_transform_ip(GstBaseTransform *trans, GstBuffer *buf) {
    GstRecord *record = GST_RECORD(trans);

    std::lock_guard<std::mutex> lock(record->record_mutex);

    if (record->is_recording && record->appsrc) {
        g_print("Pushing buffer to appsrc\n");
        GstBuffer *copy = gst_buffer_copy(buf);
        GstFlowReturn ret;
        g_signal_emit_by_name(record->appsrc, "push-buffer", copy, &ret);
        //gst_app_src_push_buffer((GstAppSrc*)record->appsrc, copy);
        gst_buffer_unref(copy);
    }

    return GST_FLOW_OK;
}

static void start_record(GstRecord *record) {
    std::lock_guard<std::mutex> lock(record->record_mutex);

    if (record->is_recording) return;

    record->pipeline = gst_pipeline_new("record-pipeline");
    record->appsrc = gst_element_factory_make("appsrc", "record-src");
    GstElement *convert = gst_element_factory_make("nvvideoconvert", "convert");
    GstElement *encoder = gst_element_factory_make("x264enc", "encoder");
    GstElement *muxer = gst_element_factory_make("mpegtsmux", "muxer");
    GstElement *sink = gst_element_factory_make("filesink", "file-output");

    g_object_set(sink, "location", "recorded.ts",  "sync", FALSE, NULL);
    g_object_set(record->appsrc, "is-live", TRUE, "format", GST_FORMAT_TIME, NULL);
    //g_object_set(record->appsrc, "stream-type", GST_APP_STREAM_TYPE_STREAM, NULL);


    gst_bin_add_many(GST_BIN(record->pipeline), record->appsrc, convert, encoder, muxer, sink, NULL);
    gst_element_link_many(record->appsrc, convert, encoder, muxer, sink, NULL);

    gst_element_set_state(record->pipeline, GST_STATE_PLAYING);
    record->is_recording = TRUE;
    g_print("Recording started.\n");
}

static void stop_record(GstRecord *record) {
    std::lock_guard<std::mutex> lock(record->record_mutex);

    if (!record->is_recording) return;

    gst_element_send_event(record->appsrc, gst_event_new_eos());
    // Wait for EOS to propagate
    // gst_element_get_state(record->pipeline, NULL, NULL, GST_CLOCK_TIME_NONE);
    // Wait for EOS to be fully processed
    GstBus *bus = gst_element_get_bus(record->pipeline);
    GstMessage *msg = gst_bus_timed_pop_filtered(
        bus, GST_CLOCK_TIME_NONE,
        static_cast<GstMessageType>(GST_MESSAGE_EOS | GST_MESSAGE_ERROR)
    );

    if (msg != nullptr) {
        gst_message_unref(msg);
    }
    gst_object_unref(bus);

    //
    gst_element_set_state(record->pipeline, GST_STATE_NULL);
    gst_object_unref(record->pipeline);

    record->pipeline = NULL;
    record->appsrc = NULL;
    record->is_recording = FALSE;

    g_print("Recording stopped.\n");
}

static void gst_record_class_init(GstRecordClass *klass) {
    GstElementClass *element_class = GST_ELEMENT_CLASS(klass);
    GstBaseTransformClass *base_transform_class = GST_BASE_TRANSFORM_CLASS(klass);

      gst_element_class_add_static_pad_template (GST_ELEMENT_CLASS (klass),
      &gst_recordplugin_src_template);
  gst_element_class_add_static_pad_template (GST_ELEMENT_CLASS (klass),
      &gst_recordplugin_sink_template);

    base_transform_class->transform_ip = GST_DEBUG_FUNCPTR(gst_record_transform_ip);

    gst_element_class_set_static_metadata(element_class,
        "Smart Recorder", "Filter/Video",
        "Records frames to MP4 on external trigger",
        "Your Name <you@example.com>");

    gst_record_signals[SIGNAL_START_RECORD] = g_signal_new("record-start",
        G_TYPE_FROM_CLASS(klass),
        G_SIGNAL_RUN_LAST,
        0, NULL, NULL, NULL, G_TYPE_NONE, 0);

    gst_record_signals[SIGNAL_STOP_RECORD] = g_signal_new("record-stop",
        G_TYPE_FROM_CLASS(klass),
        G_SIGNAL_RUN_LAST,
        0, NULL, NULL, NULL, G_TYPE_NONE, 0);
}

static void gst_record_init(GstRecord *record) {
    record->is_recording = FALSE;

    g_signal_connect(record, "record-start", G_CALLBACK(start_record), NULL);
    g_signal_connect(record, "record-stop", G_CALLBACK(stop_record), NULL);
}



static gboolean plugin_init(GstPlugin *plugin) {
    return gst_element_register(plugin, "recordplugin", GST_RANK_NONE, GST_TYPE_RECORD);
}

#define PACKAGE "recordplugin"

GST_PLUGIN_DEFINE(
    GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    nvdsgst_recordplugin,
    "Smart recorder plugin",
    plugin_init,
    "1.0",
    "LGPL",
    "DeepStream",
    "https://developer.nvidia.com"
)
