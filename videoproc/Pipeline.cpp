#include "Pipeline.h"

#include <iostream>

namespace videoproc {

Pipeline::Pipeline() {
    pipeline = gst_pipeline_new(nullptr);

    GstBus* bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline));
    gst_bus_add_watch(bus, &Pipeline::onMessage, this);
    gst_bus_set_sync_handler(bus, &Pipeline::onSyncMessage, this);
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

time_t Pipeline::getTimeSec() {
    GstClock* clock = gst_element_get_clock(pipeline);
    if (clock == nullptr) {
        return 0;
    }
    GstClockTime t = gst_clock_get_time(clock);
    return GST_TIME_AS_SECONDS(t);
}

void Pipeline::setMainLoop(std::shared_ptr<MainLoop> loop) {
    this->loop = loop;
}

gboolean Pipeline::onMessage(GstBus* bus, GstMessage* msg, gpointer data) {
    Pipeline* pipeline = static_cast<Pipeline*>(data);
    switch (GST_MESSAGE_TYPE(msg)) {
        case GST_MESSAGE_EOS: {
            pipeline->stop();
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

            pipeline->stop();
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

GstBusSyncReply Pipeline::onSyncMessage(GstBus* bus, GstMessage* msg, gpointer data) {
    return GST_BUS_PASS;
}

void Pipeline::setState(GstState state) {
    gst_element_set_state(pipeline, GST_STATE_PAUSED); // non-blocking
    gst_element_get_state(pipeline, nullptr, nullptr, GST_CLOCK_TIME_NONE); // forces the state change
}

GstElement* Pipeline::createBin(const std::vector<ElementProperties>& elements) {
    GstElement* bin = gst_bin_new(nullptr);
    GstElement* last = addElements(bin, elements);
    if (last != nullptr) {
        GstPad* pad = gst_element_get_static_pad(last, "src");
        GstPad* ghost = gst_ghost_pad_new("src", pad);
        gst_element_add_pad(bin, ghost);
        gst_object_unref(GST_OBJECT(pad));
    }
    return bin;
}

GstElement* Pipeline::addElements(GstElement* bin, const std::vector<ElementProperties>& elements, GstElement* src) {
    GstElement* last = src;
    for (const auto& elemProp : elements) {
        GstElement* elem = gst_element_factory_make(elemProp.name.c_str(), nullptr);
        gst_bin_add(GST_BIN(bin), elem);

        for (const auto& arg : elemProp.arguments) {
            g_object_set(elem, arg.first.c_str(), arg.second.c_str(), nullptr);
        }

        if (last != nullptr) {
            gst_element_link(last, elem);
        }
        last = elem;
    }
    return last;
}

GstElement* Pipeline::createVideoSinkBin(int width, int height) {
    GstElement* bin = gst_bin_new(nullptr);

    GstElement* colorspace = gst_element_factory_make("ffmpegcolorspace", nullptr);
    gst_bin_add(GST_BIN(bin), colorspace);

    GstElement* videoscale = gst_element_factory_make("videoscale", nullptr);
    gst_bin_add(GST_BIN(bin), videoscale);

    GstElement* sizecapsfilter = gst_element_factory_make("capsfilter", nullptr);
    gst_bin_add(GST_BIN(bin), sizecapsfilter);
    GstCaps* sizecaps = gst_caps_new_simple("video/x-raw-yuv",
                                            "width", G_TYPE_INT, width,
                                            "height", G_TYPE_INT, height,
                                            nullptr);
    g_object_set(sizecapsfilter, "caps", sizecaps, nullptr);

    GstElement* videosink = gst_element_factory_make("autovideosink", nullptr);
    gst_bin_add(GST_BIN(bin), videosink);

    gst_element_link_many(colorspace, videoscale, sizecapsfilter, videosink, nullptr);

    GstPad* pad = gst_element_get_static_pad(colorspace, "sink");
    GstPad* ghost = gst_ghost_pad_new("sink", pad);
    gst_element_add_pad(bin, ghost);
    gst_object_unref(GST_OBJECT(pad));

    return bin;
}

}
