#pragma once

#include "VideoHandler.h"

#include <functional>
#include <list>

namespace videoproc {

class VideoReader: public VideoHandler {
public:
    using FrameHandler = std::function<void(VideoReader*, uint8_t*, size_t)>;

    VideoReader(const std::string& name);
    virtual ~VideoReader() override;

    const std::string& getMime() const;
    int getWidth() const;
    int getHeight() const;

    std::list<FrameHandler> newPrerollCallbacks;
    std::list<FrameHandler> newBufferCallbacks;

protected:
    GstElement* appsink;

private:
    static void onNewPreroll(GstElement* appSink, VideoReader* reader);
    static void onNewBuffer(GstElement* appSink, VideoReader* reader);

    void processNewPreroll(GstBuffer* buffer);
    void processNewBuffer(GstBuffer* buffer);

    gulong prerollSigId = 0;
    gulong bufferSigId = 0;

    std::string mime;
    int width = 0;
    int height = 0;
};

}
