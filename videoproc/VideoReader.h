#pragma once

#include "Pipeline.h"

#include <functional>
#include <list>

namespace videoproc {

class VideoReader: public Pipeline {
public:
    VideoReader();
    ~VideoReader() override;

    const std::string& getMime() const;
    int getWidth() const;
    int getHeight() const;

    std::list<std::function<void(VideoReader*, uint8_t*, size_t)>> newPreroll;
    std::list<std::function<void(VideoReader*, uint8_t*, size_t)>> newBuffer;

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
};

}
