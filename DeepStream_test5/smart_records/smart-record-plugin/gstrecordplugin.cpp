#include "gstrecordplugin.h"
#include <gst/gst.h>
#include <gst/video/video.h>
#include <gst/app/gstappsink.h>
#include <gst/app/gstappsrc.h>
#include <iostream>
#include <fstream>
#include <mutex>

#include <chrono>
#include <iomanip>

std::string getFormattedTime()
{
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);

    std::tm tm_now;
    localtime_r(&now_c, &tm_now);
    std::ostringstream oss;
    oss << std::put_time(&tm_now, "%y_%m_%d_%H_%M_%S");
    return oss.str();
}

GST_DEBUG_CATEGORY_STATIC(gst_record_debug);
#define GST_CAT_DEFAULT gst_record_debug

struct _GstRecord
{
    GstBaseTransform parent;
    gboolean is_recording;
    GstElement *pipeline;
    GstElement *appsrc;
    GstElement *muxer;
    std::mutex record_mutex;
    int counter;
};

G_DEFINE_TYPE(GstRecord, gst_record, GST_TYPE_BASE_TRANSFORM)

static GstStaticPadTemplate
    gst_recordplugin_src_template = GST_STATIC_PAD_TEMPLATE("src",
                                                            GST_PAD_SRC,
                                                            GST_PAD_ALWAYS,
                                                            GST_STATIC_CAPS("ANY"));

static GstStaticPadTemplate
    gst_recordplugin_sink_template = GST_STATIC_PAD_TEMPLATE("sink",
                                                             GST_PAD_SINK,
                                                             GST_PAD_ALWAYS,
                                                             GST_STATIC_CAPS("ANY"));

enum
{
    SIGNAL_START_RECORD,
    SIGNAL_STOP_RECORD,
    LAST_SIGNAL
};

static guint gst_record_signals[LAST_SIGNAL] = {0};

static GstFlowReturn gst_record_transform_ip(GstBaseTransform *trans, GstBuffer *buf)
{
    GstRecord *record = GST_RECORD(trans);

    std::lock_guard<std::mutex> lock(record->record_mutex);
    if (record->is_recording && record->appsrc)
    {
        GstCaps *caps = gst_pad_get_current_caps(GST_BASE_TRANSFORM(trans)->sinkpad);
        if (caps)
        {
            g_object_set(record->appsrc, "caps", caps, NULL);
            gst_caps_unref(caps);
        }

        g_print("Pushing buffer to appsrc\n");
        record->counter += 1;
        g_print("FRAME %d\n", record->counter);
        GstBuffer *copy = gst_buffer_copy(buf);
        GstFlowReturn ret;
        g_signal_emit_by_name(record->appsrc, "push-buffer", copy, &ret);
        gst_buffer_unref(copy);
        if (ret != GST_FLOW_OK)
        {
            g_printerr("Failed to push buffer to appsrc\n");
        }
    }

    return GST_FLOW_OK;
}

static void start_record(GstRecord *record)
{
    std::lock_guard<std::mutex> lock(record->record_mutex);

    if (record->is_recording)
        return;

    record->pipeline = gst_pipeline_new("record-pipeline");
    record->appsrc = gst_element_factory_make("appsrc", "record-src");

    GstElement *convert = gst_element_factory_make("nvvideoconvert", "convert");
    GstElement *encoder = gst_element_factory_make("x264enc", "encoder");
    GstElement *logger = gst_element_factory_make("queue", "logger");
    GstElement *muxer = gst_element_factory_make("mp4mux", "muxer");
    GstElement *sink = gst_element_factory_make("filesink", "file-output");

    gchar *name = gst_element_get_name(record);
    std::string location = name + "__" getFormattedTime() + ".mp4";
    std::cout << "LOCATION: " << location << "\n";
    g_free(name);

    g_object_set(sink, "location", location.c_str(), "async", FALSE, NULL);
    g_object_set(record->appsrc,
                 "is-live", FALSE,
                 "format", GST_FORMAT_TIME,
                 "do-timestamp", TRUE,
                 NULL);
    record->muxer = muxer;

    gboolean ok;
    gst_bin_add_many(GST_BIN(record->pipeline), record->appsrc, convert, encoder, logger, muxer, sink, NULL);
    ok = gst_element_link_many(record->appsrc, convert, encoder, logger, muxer, sink, NULL);

    if (!ok)
    {
        g_printerr("Failed to link elements in the recording pipeline\n");
    }

    GstStateChangeReturn ret = gst_element_set_state(record->pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE)
    {
        g_printerr("Failed to set pipeline to PLAYING state\n");
    }
    else if (ret == GST_STATE_CHANGE_NO_PREROLL)
    {
        g_print("Pipeline is live and doesn't need preroll\n");
    }
    else if (ret == GST_STATE_CHANGE_ASYNC)
    {
        g_print("Pipeline is transitioning asynchronously\n");
    }
    else if (ret == GST_STATE_CHANGE_SUCCESS)
    {
        g_print(" Pipeline successfully transitioned to PLAYING\n");
    }
    record->is_recording = TRUE;
    g_print("Recording started.\n");

    GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN(record->pipeline), GST_DEBUG_GRAPH_SHOW_ALL, "record-pipeline");
}

static void stop_record(GstRecord *record)
{
    std::lock_guard<std::mutex> lock(record->record_mutex);

    if (!record->is_recording)
        return;

    gst_element_send_event(record->appsrc, gst_event_new_eos());

    // wait EOS is processed
    GstBus *bus = gst_element_get_bus(record->pipeline);
    GstMessage *msg = gst_bus_timed_pop_filtered(
        bus, GST_CLOCK_TIME_NONE,
        static_cast<GstMessageType>(GST_MESSAGE_EOS | GST_MESSAGE_ERROR));

    if (msg != nullptr)
    {
        gst_message_unref(msg);
    }
    gst_object_unref(bus);

    gst_element_set_state(record->pipeline, GST_STATE_NULL);
    gst_object_unref(record->pipeline);

    record->pipeline = NULL;
    record->appsrc = NULL;
    record->is_recording = FALSE;

    g_print("Recording stopped.\n");
}

static void gst_record_class_init(GstRecordClass *klass)
{
    GstElementClass *element_class = GST_ELEMENT_CLASS(klass);
    GstBaseTransformClass *base_transform_class = GST_BASE_TRANSFORM_CLASS(klass);

    gst_element_class_add_static_pad_template(GST_ELEMENT_CLASS(klass),
                                              &gst_recordplugin_src_template);
    gst_element_class_add_static_pad_template(GST_ELEMENT_CLASS(klass),
                                              &gst_recordplugin_sink_template);

    base_transform_class->transform_ip = GST_DEBUG_FUNCPTR(gst_record_transform_ip);

    gst_element_class_set_static_metadata(element_class,
                                          "Smart Recorder", "Filter/Video",
                                          "Records frames to MP4 on external trigger",
                                          "Anton Dorozhko");

    gst_record_signals[SIGNAL_START_RECORD] = g_signal_new("record-start",
                                                           G_TYPE_FROM_CLASS(klass),
                                                           G_SIGNAL_RUN_LAST,
                                                           0, NULL, NULL, NULL, G_TYPE_NONE, 0);

    gst_record_signals[SIGNAL_STOP_RECORD] = g_signal_new("record-stop",
                                                          G_TYPE_FROM_CLASS(klass),
                                                          G_SIGNAL_RUN_LAST,
                                                          0, NULL, NULL, NULL, G_TYPE_NONE, 0);
}

static void gst_record_init(GstRecord *record)
{
    record->is_recording = FALSE;

    g_signal_connect(record, "record-start", G_CALLBACK(start_record), NULL);
    g_signal_connect(record, "record-stop", G_CALLBACK(stop_record), NULL);
}

static gboolean plugin_init(GstPlugin *plugin)
{
    return gst_element_register(plugin, "recordplugin", GST_RANK_NONE, GST_TYPE_RECORD);
}

#define PACKAGE "recordplugin"

GST_PLUGIN_DEFINE(
    GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    nvdsgst_recordplugin,
    "Smart recorder plugin",
    plugin_init,
    "0.1",
    "LGPL",
    "ValueStream",
    "ValueStream")
