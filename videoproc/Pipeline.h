#pragma once

#include <cstdint>
#include <memory>
#include <vector>
#include <string>

#include <glib.h>
#include <gst/gst.h>

#include "MainLoop.h"
#include "ElementProperties.h"

namespace videoproc {

class Pipeline {
public:
    Pipeline();
    virtual ~Pipeline();

    virtual void play();
    virtual void pause();
    virtual void stop();

    int64_t getDuration();
    int64_t getPosition();
    void seek(int64_t position);
    time_t getTimeSec();

    void setMainLoop(std::shared_ptr<MainLoop> loop);

protected:
    void setState(GstState state);

    GstElement* createBin(const std::vector<ElementProperties>& elements);
    GstElement* addElements(GstElement* bin, const std::vector<ElementProperties>& elements, GstElement* src = nullptr);

    GstElement* createVideoSinkBin(int width, int height);

    GstElement* pipeline;
    std::shared_ptr<MainLoop> loop;

private:
    static gboolean onMessage(GstBus* bus, GstMessage* msg, gpointer data);
    static GstBusSyncReply onSyncMessage(GstBus* bus, GstMessage* msg, gpointer data);
};

}
