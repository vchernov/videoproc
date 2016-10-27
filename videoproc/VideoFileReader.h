#pragma once

#include "VideoReader.h"

namespace videoproc {

class VideoFileReader : public VideoReader {
public:
    VideoFileReader(const std::string& location);

    std::string getLocation();

protected:
    GstElement* fileSrc;
};

}
