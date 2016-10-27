#include <iostream>

#include "../../videoproc/MainLoop.h"
#include "../../videoproc/TeePipeline.h"

using namespace videoproc;

int main(int argc, char** argv) {
    std::cout << "start" << std::endl;

    gst_init(&argc, &argv);

    std::vector<ElementProperties> sourceElements;
    ElementProperties webcamElem;
    webcamElem.name = "v4l2src";
    webcamElem.arguments["device"] = "/dev/video0";
    sourceElements.push_back(webcamElem);

    auto mainLoop = std::make_shared<MainLoop>();
    auto webcamPipeline = std::make_unique<TeePipeline>(sourceElements);
    webcamPipeline->setMainLoop(mainLoop);
    webcamPipeline->createViewBranch(1024, 768);
    webcamPipeline->play();
    webcamPipeline->attachAllBranches();
    mainLoop->run();

    std::cout << "end" << std::endl;
    return 0;
}
