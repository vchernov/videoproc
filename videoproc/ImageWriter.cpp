#include "ImageWriter.h"

#include <string.h>

namespace videoproc {

std::unique_ptr<ImageWriter> ImageWriter::createBmpWriter() {
    auto writer = std::make_unique<ImageWriter>();

    GstElement* encoder = gst_element_factory_make("ffenc_bmp", nullptr);
    g_assert(encoder);

    gst_bin_add(GST_BIN(writer->pipeline), encoder);
    gst_element_link_many(writer->appSrc, encoder, writer->fileSink, nullptr);
    return writer;
}

std::unique_ptr<ImageWriter> ImageWriter::createJpegWriter() {
    auto writer = std::make_unique<ImageWriter>();

    GstElement* encoder = gst_element_factory_make("jpegenc", nullptr);
    g_assert(encoder);

    gst_bin_add(GST_BIN(writer->pipeline), encoder);
    gst_element_link_many(writer->appSrc, encoder, writer->fileSink, nullptr);
    return writer;
}

ImageWriter::ImageWriter() {
    appSrc = gst_element_factory_make("appsrc", nullptr);
    g_assert(appSrc);
    gst_bin_add(GST_BIN(pipeline), appSrc);
    g_object_set (G_OBJECT(appSrc), "stream-type", 0, nullptr);
    g_object_set(appSrc, "num-buffers", 1, nullptr);

    fileSink = gst_element_factory_make("filesink", nullptr);
    g_assert(fileSink);
    gst_bin_add(GST_BIN(pipeline), fileSink);
}

void ImageWriter::setLocation(const std::string& location) {
    g_object_set(fileSink, "location", location.c_str(), nullptr);
}

void ImageWriter::write(GstBuffer* buffer) {
    play();

    GstCaps* caps = gst_buffer_get_caps(buffer);
    g_object_set(appSrc, "caps", caps, nullptr);
    gst_caps_unref(caps);

    GstFlowReturn ret;
    g_signal_emit_by_name(appSrc, "push-buffer", buffer, &ret);
    g_assert(ret == GST_FLOW_OK);
}

void ImageWriter::write(uint8_t* pixels, size_t len, GstCaps* caps) {
    GstBuffer* buffer = gst_buffer_new_and_alloc(len);
    gst_buffer_set_caps(buffer, caps);
    memcpy(GST_BUFFER_DATA(buffer), pixels, len);

    write(buffer);

    gst_buffer_unref(buffer);
}

void ImageWriter::write(uint8_t* pixels, int width, int height) {
    GstCaps* caps = gst_caps_new_simple(
                        "video/x-raw-rgb",
                        "bpp", G_TYPE_INT, 24,
                        "depth", G_TYPE_INT, 24,
                        "endianness", G_TYPE_INT, 4321,
                        "red_mask", G_TYPE_INT, 255,
                        "green_mask", G_TYPE_INT, 65280,
                        "blue_mask", G_TYPE_INT, 16711680,
                        "framerate", GST_TYPE_FRACTION, 1, 1,
                        "width", G_TYPE_INT, width,
                        "height", G_TYPE_INT, height,
                        nullptr);

    write(pixels, width * height * 3, caps);

    gst_caps_unref(caps);
}

}
