#include "VideoReader.h"

#include <iostream>

namespace videoproc {

VideoReader::VideoReader(const std::string& name) : VideoHandler(name) {
    appsink = gst_element_factory_make("appsink", nullptr);
    g_assert(appsink);

    gst_bin_add(GST_BIN(pipeline), appsink);
    g_object_set(appsink, "emit-signals", true, "sync", true, nullptr);
    prerollSigId = g_signal_connect(appsink, "new-preroll", G_CALLBACK(onNewPreroll), this);
    bufferSigId = g_signal_connect(appsink, "new-buffer", G_CALLBACK(onNewBuffer), this);
}

VideoReader::~VideoReader() {
    g_signal_handler_disconnect(appsink, prerollSigId);
    g_signal_handler_disconnect(appsink, bufferSigId);
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
    for (const auto& callback : newPrerollCallbacks) {
        callback(this, data, size);
    }
}

void VideoReader::processNewBuffer(GstBuffer* buffer) {
    uint8_t* data = GST_BUFFER_DATA(buffer);
    size_t size = GST_BUFFER_SIZE(buffer);
    for (const auto& callback : newBufferCallbacks) {
        callback(this, data, size);
    }
}

}
