#pragma once

#include "IRenderable.hpp"
#include "SfmCameraRenderable.hpp"
#include "SfmDataLayer.hpp"
#include "SfmPointRenderable.hpp"

class SfmDataRenderable : public IRenderable
{
public:
    void sync(LayerItem *layer, SceneView *view) override;
    void initialize(QRhi *rhi, QRhiRenderPassDescriptor *rpDesc) override;
    void prepare(QRhiResourceUpdateBatch *batch, const SceneState &state) override;
    void render(QRhiCommandBuffer *cb, const SceneState &state) override;

private:
    SfmPointRenderable _points;
    SfmCameraRenderable _cameras;
};