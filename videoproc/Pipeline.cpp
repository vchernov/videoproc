#include "Pipeline.h"

#include <iostream>

namespace videoproc {

Pipeline::Pipeline(const std::string& name) {
    pipeline = gst_pipeline_new(name.c_str());

    GstBus* bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline));
    gst_bus_add_watch(bus, &Pipeline::onMessage, this);
    gst_bus_enable_sync_message_emission(bus);
    gst_object_unref(GST_OBJECT(bus));
}

Pipeline::~Pipeline() {
    stop();

    gst_object_unref(GST_OBJECT(pipeline));
}

void Pipeline::play() {
    gst_element_set_state(pipeline, GST_STATE_PLAYING);
}

void Pipeline::pause() {
    gst_element_set_state(pipeline, GST_STATE_PAUSED);
}

void Pipeline::stop() {
    gst_element_set_state(pipeline, GST_STATE_NULL);

    if (loop) {
        loop->quit();
    }
}

int64_t Pipeline::getDuration() {
    gint64 duration = 0;
    GstFormat format = GST_FORMAT_TIME;
    gst_element_query_duration(pipeline, &format, &duration);
    return (int64_t)duration;
}

int64_t Pipeline::getPosition() {
    gint64 position = 0;
    GstFormat format = GST_FORMAT_TIME;
    gst_element_query_position(pipeline, &format, &position);
    return (int64_t)position;
}

void Pipeline::seek(int64_t position) {
    gst_element_seek_simple(pipeline, GST_FORMAT_TIME, GST_SEEK_FLAG_FLUSH, (gint64)position);
}

void Pipeline::setMainLoop(std::shared_ptr<MainLoop> loop) {
    this->loop = loop;
}

void Pipeline::setState(GstState state) {
    gst_element_set_state(pipeline, GST_STATE_PAUSED); // non-blocking
    gst_element_get_state(pipeline, nullptr, nullptr, GST_CLOCK_TIME_NONE); // forces the state change
}

gboolean Pipeline::onMessage(GstBus* bus, GstMessage* msg, gpointer data) {
    Pipeline* handler = static_cast<Pipeline*>(data);
    switch (GST_MESSAGE_TYPE(msg)) {
        case GST_MESSAGE_EOS: {
            handler->stop();
            break;
        }
        case GST_MESSAGE_ERROR: {
            gchar* debug;
            GError* error;
            gst_message_parse_error(msg, &error, &debug);
            std::cerr << error->message << std::endl;
            std::cerr << debug << std::endl;
            g_free(debug);
            g_error_free(error);
            handler->stop();
            break;
        }
        case GST_MESSAGE_WARNING: {
            gchar* debug;
            GError* error;
            gst_message_parse_warning(msg, &error, &debug);
            std::cerr << error->message << std::endl;
            std::cerr << debug << std::endl;
            g_free(debug);
            g_error_free(error);
            break;
        }
    }
    return true;
}

void Pipeline::deferredLink(GstElement* elem, GstPad* pad, GstElement* sinkElem) {
    GstPad* sinkPad = gst_element_get_static_pad(sinkElem, "sink");
    gst_pad_link(pad, sinkPad);
    gst_object_unref(sinkPad);
}

}
