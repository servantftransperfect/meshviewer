#include "SfmDataRenderable.hpp"

#include "LayerItem.hpp"

void SfmDataRenderable::sync(LayerItem *layer, SceneView *view)
{
    Q_UNUSED(view)

    auto *sfmLayer = qobject_cast<SfmDataLayer *>(layer);
    if (!sfmLayer)
    {
        return;
    }

    _cameras.setSelectedCamera(sfmLayer->selectedCameraIndex());

    if (!sfmLayer->dataDirty())
    {
        return;
    }

    const SfmDataRenderData &renderData = sfmLayer->renderData();
    _points.setData(renderData.points, renderData.pointRadius);
    _cameras.setData(renderData.cameras, renderData.cameraScale);

    sfmLayer->clearDataDirty();
}

void SfmDataRenderable::initialize(QRhi *rhi, QRhiRenderPassDescriptor *rpDesc)
{
    _points.initialize(rhi, rpDesc);
    _cameras.initialize(rhi, rpDesc);
}

void SfmDataRenderable::prepare(QRhiResourceUpdateBatch *batch, const SceneState &state)
{
    _points.prepare(batch, state);
    _cameras.prepare(batch, state);
}

void SfmDataRenderable::render(QRhiCommandBuffer *cb, const SceneState &state)
{
    _points.render(cb, state);
    _cameras.render(cb, state);
}