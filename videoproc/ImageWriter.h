#pragma once

#include "Pipeline.h"

#include <memory>

namespace videoproc {

class ImageWriter : public Pipeline {
public:
    static std::unique_ptr<ImageWriter> createBmpWriter(const std::string& name);
    static std::unique_ptr<ImageWriter> createJpegWriter(const std::string& name);

    ImageWriter(const std::string& name);
    virtual ~ImageWriter() = default;

    void setLocation(const std::string& location);

    void write(GstBuffer* buffer);
    void write(uint8_t* pixels, size_t len, GstCaps* caps);
    void write(uint8_t* pixels, int width, int height);

protected:
    GstElement* appSrc;
    GstElement* fileSink;
};

}
