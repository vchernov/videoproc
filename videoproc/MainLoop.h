#pragma once

#include <glib.h>

namespace videoproc {

class MainLoop {
public:
    MainLoop();
    virtual ~MainLoop();

    void run();
    void quit();

private:
    GMainLoop* loop;
};

}
