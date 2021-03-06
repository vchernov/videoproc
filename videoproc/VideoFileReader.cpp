#include "VideoFileReader.h"

namespace videoproc {

VideoFileReader::VideoFileReader(const std::string& location) {
    fileSrc = gst_element_factory_make("filesrc", nullptr);
    g_assert(fileSrc);
    gst_bin_add(GST_BIN(pipeline), fileSrc);
    g_object_set(fileSrc, "location", location.c_str(), nullptr);
}

std::string VideoFileReader::getLocation() {
    gchar* val;
    g_object_get(fileSrc, "location", &val, nullptr);
    std::string location;
    if (val) {
        location = val;
    }
    return location;
}

}
