#include "VideoHandler.h"

#include <iostream>

namespace videoproc {

VideoHandler::VideoHandler(const std::string& name) {
    pipeline = gst_pipeline_new(name.c_str());

    GstBus* bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline));
    gst_bus_add_watch(bus, &VideoHandler::onMessage, this);
    gst_bus_enable_sync_message_emission(bus);
    gst_object_unref(GST_OBJECT(bus));
}

VideoHandler::~VideoHandler() {
    stop();

    gst_object_unref(GST_OBJECT(pipeline));
}

void VideoHandler::play() {
    gst_element_set_state(pipeline, GST_STATE_PLAYING);
}

void VideoHandler::pause() {
    gst_element_set_state(pipeline, GST_STATE_PAUSED);
}

void VideoHandler::stop() {
    gst_element_set_state(pipeline, GST_STATE_NULL);

    if (loop) {
        loop->quit();
    }
}

int64_t VideoHandler::getDuration() {
    gint64 duration = 0;
    GstFormat format = GST_FORMAT_TIME;
    gst_element_query_duration(pipeline, &format, &duration);
    return (int64_t)duration;
}

int64_t VideoHandler::getPosition() {
    gint64 position = 0;
    GstFormat format = GST_FORMAT_TIME;
    gst_element_query_position(pipeline, &format, &position);
    return (int64_t)position;
}

void VideoHandler::seek(int64_t position) {
    gst_element_seek_simple(pipeline, GST_FORMAT_TIME, GST_SEEK_FLAG_FLUSH, (gint64)position);
}

void VideoHandler::setMainLoop(std::shared_ptr<MainLoop> loop) {
    this->loop = loop;
}

void VideoHandler::setState(GstState state) {
    gst_element_set_state(pipeline, GST_STATE_PAUSED); // non-blocking
    gst_element_get_state(pipeline, NULL, NULL, GST_CLOCK_TIME_NONE); // forces the state change
}

gboolean VideoHandler::onMessage(GstBus* bus, GstMessage* msg, gpointer data) {
    VideoHandler* handler = static_cast<VideoHandler*>(data);
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

void VideoHandler::deferredLink(GstElement* elem, GstPad* pad, GstElement* sinkElem) {
    GstPad* sinkPad = gst_element_get_static_pad(sinkElem, "sink");
    gst_pad_link(pad, sinkPad);
    gst_object_unref(sinkPad);
}

}
