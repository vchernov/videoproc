#pragma once

#include "Pipeline.h"

#include <functional>
#include <list>

namespace videoproc {

class VideoReader: public Pipeline {
public:
    using FrameHandler = std::function<void(VideoReader*, uint8_t*, size_t)>;
    using FrameHandlerContainer = std::list<FrameHandler>;

    VideoReader(const std::string& name);
    virtual ~VideoReader() override;

    const std::string& getMime() const;
    int getWidth() const;
    int getHeight() const;

    FrameHandlerContainer& getNewPrerollCallbacks();
    FrameHandlerContainer& getNewBufferCallbacks();

protected:
    GstElement* appSink;

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

    FrameHandlerContainer newPrerollCallbacks;
    FrameHandlerContainer newBufferCallbacks;
};

}
