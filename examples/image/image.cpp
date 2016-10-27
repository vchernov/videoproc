#include <iostream>

#include "../../videoproc/MainLoop.h"
#include "../../videoproc/ImageWriter.h"

using namespace videoproc;

int main(int argc, char** argv) {
    std::cout << "start" << std::endl;

    gst_init(&argc, &argv);

    int width = 256;
    int height = 256;
    uint8_t* img = new uint8_t[width * height * 3];
    for (int y = 0; y < height; y++) {
        int row = width * y * 3;
        for (int x = 0; x < width; x++) {
            int offset = row + x * 3;
            img[offset] = 0;
            img[offset + 1] = (uint8_t)x;
            img[offset + 2] = (uint8_t)y;
        }
    }

    auto writer = ImageWriter::createBmpWriter();
    auto mainLoop = std::make_shared<MainLoop>();
    writer->setMainLoop(mainLoop);

    writer->setLocation("image.bmp");
    writer->write(img, width, height);

    mainLoop->run();

    delete[] img;

    std::cout << "end" << std::endl;
    return 0;
}
