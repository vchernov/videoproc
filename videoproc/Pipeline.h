#pragma once

#include <cstdint>
#include <memory>
#include <string>

#include <glib.h>
#include <gst/gst.h>

#include "MainLoop.h"

namespace videoproc {

class Pipeline {
public:
    Pipeline(const std::string& name);
    virtual ~Pipeline();

    void play();
    void pause();
    void stop();

    int64_t getDuration();
    int64_t getPosition();
    void seek(int64_t position);

    void setMainLoop(std::shared_ptr<MainLoop> loop);

protected:
    static void deferredLink(GstElement* elem, GstPad* pad, GstElement* sinkElem);

    void setState(GstState state);

    GstElement* pipeline;
    std::shared_ptr<MainLoop> loop;

private:
    static gboolean onMessage(GstBus* bus, GstMessage* msg, gpointer data);
};

}
