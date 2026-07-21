#pragma once

#include <QQuickRhiItem>
#include <rhi/qrhi.h>
#include <QList>
#include <memory>
#include <vector>

#include "IRenderable.hpp"
#include "SceneState.hpp"

class LayerItem;
class SceneView;

/**
 * @brief QQuickRhiItemRenderer that orchestrates a set of IRenderable objects.
 *
 * Per-frame loop:
 *  - synchronize(): fills SceneState; rebuilds renderables when layers change;
 *                   calls IRenderable::sync() on each renderable.
 *  - initialize(): calls IRenderable::initialize() on each renderable.
 *  - render(): calls IRenderable::prepare() then IRenderable::render() within a single pass.
 *              Invisible layers (LayerItem::visible()==false) skip render() but not prepare().
 */
class SceneRenderer : public QQuickRhiItemRenderer
{
public:
    SceneRenderer();
    ~SceneRenderer() override;

    void initialize(QRhiCommandBuffer *cb) override;
    void synchronize(QQuickRhiItem *item) override;
    void render(QRhiCommandBuffer *cb) override;

private:
    void rebuildRenderables(const QList<LayerItem *> &layers);

    QRhi *_rhi = nullptr;
    QRhiRenderPassDescriptor *_rpDesc = nullptr;

    SceneState _state;
    QList<LayerItem *> _layerItems;
    std::vector<std::unique_ptr<IRenderable>> _renderables;
};
