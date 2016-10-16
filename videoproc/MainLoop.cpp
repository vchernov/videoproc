#include "MainLoop.h"

namespace videoproc {

MainLoop::MainLoop() {
    loop = g_main_loop_new(nullptr, false);
}

MainLoop::~MainLoop() {
    g_main_loop_unref(loop);
}

void MainLoop::run() {
    g_main_loop_run(loop);
}

void MainLoop::quit() {
    g_main_loop_quit(loop);
}

}
