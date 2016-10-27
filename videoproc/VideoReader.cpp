#include "VideoReader.h"

#include <iostream>

namespace videoproc {

VideoReader::VideoReader() {
    appSink = gst_element_factory_make("appsink", nullptr);
    g_assert(appSink);

    gst_bin_add(GST_BIN(pipeline), appSink);
    g_object_set(appSink, "emit-signals", true, "sync", true, nullptr);
    prerollSigId = g_signal_connect(appSink, "new-preroll", G_CALLBACK(onNewPreroll), this);
    bufferSigId = g_signal_connect(appSink, "new-buffer", G_CALLBACK(onNewBuffer), this);
}

VideoReader::~VideoReader() {
    g_signal_handler_disconnect(appSink, prerollSigId);
    g_signal_handler_disconnect(appSink, bufferSigId);
}

const std::string& VideoReader::getMime() const {
    return mime;
}

int VideoReader::getWidth() const {
    return width;
}

int VideoReader::getHeight() const {
    return height;
}

void VideoReader::onNewPreroll(GstElement* appSink, VideoReader* reader) {
    GstBuffer* buffer;
    g_signal_emit_by_name(appSink, "pull-preroll", &buffer);
    if (buffer) {
        reader->processNewPreroll(buffer);
        gst_buffer_unref(buffer);
    } else {
        std::cerr << "empty buffer" << std::endl;
    }
}

void VideoReader::onNewBuffer(GstElement* appSink, VideoReader* reader) {
    GstBuffer* buffer;
    g_signal_emit_by_name(appSink, "pull-buffer", &buffer);
    if (buffer) {
        reader->processNewBuffer(buffer);
        gst_buffer_unref(buffer);
    } else {
        std::cerr << "empty buffer" << std::endl;
    }
}

void VideoReader::processNewPreroll(GstBuffer* buffer) {
    GstCaps* caps = gst_buffer_get_caps(buffer);
    GstStructure* props = gst_caps_get_structure(caps, 0);

    mime = gst_structure_get_name(props);
    gst_structure_get_int(props, "width", &width);
    gst_structure_get_int(props, "height", &height);

    gst_caps_unref(caps);

    uint8_t* data = GST_BUFFER_DATA(buffer);
    size_t size = GST_BUFFER_SIZE(buffer);
    for (const auto& callback : newPreroll) {
        callback(this, data, size);
    }
}

void VideoReader::processNewBuffer(GstBuffer* buffer) {
    uint8_t* data = GST_BUFFER_DATA(buffer);
    size_t size = GST_BUFFER_SIZE(buffer);
    for (const auto& callback : newBuffer) {
        callback(this, data, size);
    }
}

}
