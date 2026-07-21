#pragma once

#include <rhi/qrhi.h>
#include "SceneState.hpp"

class LayerItem;
class SceneView;

/**
 * @brief Interface for an object that participates in the scene render loop.
 *
 * Lifecycle per frame, driven by SceneRenderer:
 * @code
 *   sync() → initialize() → prepare() → [beginPass] → render() → [endPass]
 * @endcode
 */
class IRenderable
{
public:
    virtual ~IRenderable() = default;

    /**
     * @brief Called from SceneRenderer::synchronize() while the GUI thread is blocked.
     *
     * Override to pull data from @p layer and scene-wide state from @p view.
     */
    virtual void sync(LayerItem *layer, SceneView *view) { Q_UNUSED(layer) Q_UNUSED(view) }

    /**
     * @brief Called every frame; rebuild the pipeline and GPU buffers here on RHI device change.
     * @param rhi    The current RHI instance.
     * @param rpDesc Render-pass descriptor borrowed from the render target.
     */
    virtual void initialize(QRhi *rhi, QRhiRenderPassDescriptor *rpDesc) = 0;

    /**
     * @brief Called before beginPass(); upload static data and update dynamic uniforms.
     * @param batch  Resource update batch to record uploads into.
     * @param state  Current frame snapshot shared across all renderables.
     */
    virtual void prepare(QRhiResourceUpdateBatch *batch, const SceneState &state) = 0;

    /**
     * @brief Called inside an active render pass; issue draw calls only.
     * @param cb     Active command buffer.
     * @param state  Current frame snapshot shared across all renderables.
     */
    virtual void render(QRhiCommandBuffer *cb, const SceneState &state) = 0;
};
