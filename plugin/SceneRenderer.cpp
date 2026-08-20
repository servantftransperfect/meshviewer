#include "SceneRenderer.hpp"
#include "SceneView.hpp"
#include "LayerItem.hpp"

#include "MeshPicker.hpp"

#include <QMetaObject>
#include <QPointer>

#include <limits>

namespace {

struct LayerPickSelection
{
    QPointer<LayerItem> layer;
    LayerPickResult result;
};

LayerPickSelection pickClosestLayer(const QList<LayerItem *> &layers,
                                    const Ray &ray)
{
    LayerPickSelection bestSelection;
    bestSelection.result.distance = std::numeric_limits<float>::max();

    for (LayerItem *layer : layers)
    {
        if (!layer || !layer->visible() || !layer->picking() || !layer->canPick())
        {
            continue;
        }

        const LayerPickResult candidate = layer->pick(ray);
        if (candidate.hit && candidate.distance > 0.0f && candidate.distance < bestSelection.result.distance)
        {
            bestSelection.layer = layer;
            bestSelection.result = candidate;
        }
    }

    if (!bestSelection.result.hit)
    {
        return {};
    }

    return bestSelection;
}

}

SceneRenderer::SceneRenderer() = default;
SceneRenderer::~SceneRenderer() = default;

void SceneRenderer::rebuildRenderables(const QList<LayerItem *> &layers)
{
    _renderables.clear();
    for (LayerItem *layer : layers)
        _renderables.push_back(layer->createRenderable());

    // Initialize any newly created renderables if RHI is already available.
    if (_rhi && _rpDesc)
    {
        for (auto &r : _renderables)
        {
            if (r)
                r->initialize(_rhi, _rpDesc);
        }
    }
}

void SceneRenderer::initialize(QRhiCommandBuffer *cb)
{
    Q_UNUSED(cb)

    _rhi   = rhi();
    _rpDesc = renderTarget()->renderPassDescriptor();

    for (auto &r : _renderables)
    {
        if (r)
        {
            r->initialize(_rhi, _rpDesc);
        }
    }
}

void SceneRenderer::synchronize(QQuickRhiItem *item)
{
    SceneView *view = static_cast<SceneView *>(item);

    // Rebuild renderables whenever the layer list changes.
    const QList<LayerItem *> &newLayers = view->layerList();
    if (newLayers != _layerItems)
    {
        _layerItems = newLayers;
        rebuildRenderables(_layerItems);
    }

    _state.viewportWidth  = static_cast<int>(view->width());
    _state.viewportHeight = static_cast<int>(view->height());

    view->getCameraInfo().setViewportWidth(_state.viewportWidth);
    view->getCameraInfo().setViewportHeight(_state.viewportHeight);

    for (int i = 0; i < _layerItems.size(); ++i)
    {
        if (_renderables[i])
        {
            _renderables[i]->sync(_layerItems[i], view);
        }
    }

    view->getCameraInfo().setBackendProjectionMatrix(_rhi ? _rhi->clipSpaceCorrMatrix() : QMatrix4x4{});

    const QMatrix4x4 model = view->getMotionInfo().getMatrix();
    const QMatrix4x4 proj  = view->getCameraInfo().getProjectionMatrix();

    _state.viewProjection = proj * model;
    _state.normalMatrix   = model.inverted().transposed();
    _state.projectionScaleY = proj(1, 1);

    const SceneView::PendingPickRequest pickRequest = view->takePendingPickRequest();
    if (pickRequest.pending && _state.viewportWidth > 0 && _state.viewportHeight > 0)
    {
        const Ray ray = MeshPicker::unprojectRay(proj,
                                                 model,
                                                 pickRequest.mousePos,
                                                 static_cast<float>(_state.viewportWidth),
                                                 static_cast<float>(_state.viewportHeight));

        const LayerPickSelection selection = pickClosestLayer(_layerItems, ray);

        QMetaObject::invokeMethod(
            view,
            [view, selection]() {
                view->applyLayerPick(selection.layer, selection.result);
            },
            Qt::QueuedConnection);
    }
}

void SceneRenderer::render(QRhiCommandBuffer *cb)
{
    QRhiResourceUpdateBatch *batch = _rhi->nextResourceUpdateBatch();
    for (auto &r : _renderables)
    {
        if (r)
            r->prepare(batch, _state);
    }

    cb->resourceUpdate(batch);

    const QColor clearColor = QColor::fromRgbF(0.15f, 0.15f, 0.15f, 1.0f);
    cb->beginPass(renderTarget(), clearColor, { 1.0f, 0 });
    for (int i = 0; i < static_cast<int>(_renderables.size()); ++i)
    {
        if (_renderables[i] && _layerItems[i]->visible())
            _renderables[i]->render(cb, _state);
    }

    cb->endPass();
}