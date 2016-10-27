#include "TeePipeline.h"

namespace videoproc {

TeePipeline::TeePipeline(const std::vector<ElementProperties>& sourceElements) {
    GstElement* source = createBin(sourceElements);
    gst_bin_add(GST_BIN(pipeline), source);

    tee = gst_element_factory_make("tee", nullptr);
    gst_bin_add(GST_BIN(pipeline), tee);

    GstElement* fakeSink = gst_element_factory_make("fakesink", nullptr);
    g_object_set(fakeSink, "async", false, nullptr);
    gst_bin_add(GST_BIN(pipeline), fakeSink);

    gst_element_link_many(source, tee, fakeSink, nullptr);
}

void TeePipeline::createViewBranch(int width, int height, const std::vector<ElementProperties>& decodeElements) {
    GstElement* viewBin = gst_bin_new(nullptr);

    GstElement* queueView = gst_element_factory_make("queue", nullptr);
    gst_bin_add(GST_BIN(viewBin), queueView);

    GstElement* lastDecode = addElements(viewBin, decodeElements, queueView);

    GstElement* videoSink = createVideoSinkBin(width, height);
    gst_bin_add(GST_BIN(viewBin), videoSink);
    gst_element_link(lastDecode, videoSink);

    GstPad* pad = gst_element_get_static_pad(queueView, "sink");
    GstPad* ghost = gst_ghost_pad_new("sink", pad);
    gst_element_add_pad(viewBin, ghost);
    gst_object_unref(GST_OBJECT(pad));

    branches[viewBin] = nullptr;
}

void TeePipeline::createViewBranch(int width, int height) {
    std::vector<ElementProperties> decodeElements;
    createViewBranch(width, height, decodeElements);
}

void TeePipeline::attachAllBranches() {
    for (auto& it : branches) {
        if (it.second == nullptr) {
            it.second = attach(it.first);
        }
    }
}

void TeePipeline::detachAllBranches() {
    for (auto& it : branches) {
        detach(it.first, it.second);
    }
}

void TeePipeline::removeAllBranches() {
    for (auto& it : branches) {
        remove(it.first, it.second);
    }
}

void TeePipeline::stop() {
    Pipeline::stop();
    removeAllBranches();
}

GstPad* TeePipeline::attach(GstElement* elem) {
    GstPad* srcPad = gst_element_get_request_pad(tee, "src%d");
    GstPad* sinkPad = gst_element_get_static_pad(elem, "sink");

    gst_pad_set_blocked(srcPad, true);

    gst_bin_add(GST_BIN(pipeline), elem);
    gst_element_set_state(elem, GST_STATE_READY);
    gst_pad_link(srcPad, sinkPad);
    gst_element_sync_state_with_parent(elem);

    gst_pad_set_blocked(srcPad, false);

    gst_object_unref(GST_OBJECT(sinkPad));
    return srcPad;
}

void TeePipeline::detach(GstElement* elem, GstPad*& srcPad) {
    if (srcPad == nullptr) {
        return;
    }

    GstPad* sinkPad = gst_element_get_static_pad(elem, "sink");

    gst_pad_set_blocked(srcPad, true);

    gst_pad_unlink(srcPad, sinkPad);
    gst_bin_remove(GST_BIN(pipeline), elem);
    gst_element_set_state(elem, GST_STATE_NULL);

    gst_pad_set_blocked(srcPad, false);

    gst_element_remove_pad(tee, srcPad);
    gst_object_unref(GST_OBJECT(srcPad));
    srcPad = nullptr;

    gst_object_unref(GST_OBJECT(sinkPad));
}

void TeePipeline::remove(GstElement* elem, GstPad*& srcPad) {
    if (gst_bin_get_by_name(GST_BIN(pipeline), gst_element_get_name(elem)) != nullptr) {
        gst_bin_remove(GST_BIN(pipeline), elem);
    }
    if (srcPad != nullptr) {
        gst_object_unref(GST_OBJECT(srcPad));
        srcPad = nullptr;
    }
}

}
