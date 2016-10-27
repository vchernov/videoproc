#pragma once

#include "Pipeline.h"

namespace videoproc {

class TeePipeline : public Pipeline {
public:
    TeePipeline(const std::vector<ElementProperties>& sourceElements);

    void createViewBranch(int width, int height, const std::vector<ElementProperties>& decodeElements);
    void createViewBranch(int width, int height);

    void attachAllBranches();
    void detachAllBranches();
    void removeAllBranches();

    void stop() override;

protected:
    GstPad* attach(GstElement* elem);
    void detach(GstElement* elem, GstPad*& srcPad);
    void remove(GstElement* elem, GstPad*& srcPad);

    GstElement* tee;

    std::unordered_map<GstElement*, GstPad*> branches;
};

}
