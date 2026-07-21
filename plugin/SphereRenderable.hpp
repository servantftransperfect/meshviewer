#pragma once

#include "IRenderable.hpp"
#include "vertex.hpp"

#include <QMatrix4x4>
#include <QVector3D>
#include <memory>
#include <vector>

/**
 * @brief IRenderable that draws any number of solid spheres via GPU instancing.
 *
 * A single UV-sphere mesh is uploaded once. Each call to setPositions() updates
 * a per-instance buffer holding the world-space centre of each sphere.
 * The MVP uniform contains viewProjection only; instance offsets are added in
 * the vertex shader.
 *
 * Thread safety: setPositions() must be called from the render thread
 * (e.g. inside SceneRenderer::synchronize()).
 */
class SphereRenderable : public IRenderable
{
public:
    void sync(LayerItem *layer, SceneView *view) override;
    void initialize(QRhi *rhi, QRhiRenderPassDescriptor *rpDesc) override;
    void prepare(QRhiResourceUpdateBatch *batch, const SceneState &state) override;
    void render(QRhiCommandBuffer *cb, const SceneState &state) override;

private:
    void setPositions(const std::vector<QVector3D> &positions);
    void buildPipeline();
    void buildGeometry(QRhiResourceUpdateBatch *batch);
    void rebuildInstanceBuffer(QRhiResourceUpdateBatch *batch);

    QRhi *_rhi = nullptr;
    QRhiRenderPassDescriptor *_rpDesc = nullptr;

    std::vector<QVector3D> _positions;

    std::unique_ptr<QRhiBuffer> _vertexBuffer;
    std::unique_ptr<QRhiBuffer> _indexBuffer;
    std::unique_ptr<QRhiBuffer> _instanceBuffer; /**< Per-instance world offsets. */
    std::unique_ptr<QRhiBuffer> _uniformBuffer;
    std::unique_ptr<QRhiShaderResourceBindings> _srb;
    std::unique_ptr<QRhiGraphicsPipeline> _pipeline;

    quint32 _indexCount = 0;
    quint32 _instanceCount = 0;

    bool _pipelineDirty = true;
    bool _geomDirty = true;
    bool _instancesDirty = false;
};
