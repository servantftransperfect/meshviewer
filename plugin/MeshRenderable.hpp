#pragma once

#include "IRenderable.hpp"
#include "vertex.hpp"
#include <memory>

/** @brief IRenderable that draws the scene mesh with Blinn-Phong shading. */
class MeshRenderable : public IRenderable
{
public:
    /** @brief Pulls new mesh data from @p layer (cast to MeshLayer*). */
    void sync(LayerItem *layer, SceneView *view) override;

    void initialize(QRhi *rhi, QRhiRenderPassDescriptor *rpDesc) override;
    void prepare(QRhiResourceUpdateBatch *batch, const SceneState &state) override;
    void render(QRhiCommandBuffer *cb, const SceneState &state) override;

private:
    /** @brief Creates the mesh RHI pipeline and SRB. */
    void buildPipeline();
    /** @brief Allocates and uploads vertex/index buffers. */
    void rebuildBuffers(QRhiResourceUpdateBatch *batch);

    QRhi *_rhi = nullptr;
    QRhiRenderPassDescriptor *_rpDesc = nullptr;

    std::unique_ptr<QRhiBuffer> _vertexBuffer;
    std::unique_ptr<QRhiBuffer> _indexBuffer;
    std::unique_ptr<QRhiBuffer> _uniformBuffer;
    std::unique_ptr<QRhiShaderResourceBindings> _srb;
    std::unique_ptr<QRhiGraphicsPipeline> _pipeline;

    QVector<Vertex> _vertices;
    QVector<quint32> _indices;

    bool _pipelineDirty = true;
    bool _buffersDirty = true;
    QMatrix4x4 _modelMatrix;
};
