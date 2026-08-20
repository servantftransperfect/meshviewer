#pragma once

#include "IRenderable.hpp"
#include "SfmDataLayer.hpp"
#include "vertex.hpp"

#include <memory>
#include <optional>
#include <QVector4D>
#include <vector>

class SfmPointRenderable : public IRenderable
{
public:
    void setData(std::vector<SfmDataPointInstance> points, float pointRadius);

    void initialize(QRhi *rhi, QRhiRenderPassDescriptor *rpDesc) override;
    void prepare(QRhiResourceUpdateBatch *batch, const SceneState &state) override;
    void render(QRhiCommandBuffer *cb, const SceneState &state) override;

private:
    void buildPipeline();
    void rebuildInstanceBuffer(QRhiResourceUpdateBatch *batch);

    struct UniformData
    {
        QMatrix4x4 mvp;
        QVector4D spriteInfo;
    };

    QRhi *_rhi = nullptr;
    QRhiRenderPassDescriptor *_rpDesc = nullptr;

    std::vector<SfmDataPointInstance> _points;
    float _pointRadius = 0.01f;

    std::unique_ptr<QRhiBuffer> _instanceBuffer;
    std::unique_ptr<QRhiBuffer> _uniformBuffer;
    std::unique_ptr<QRhiShaderResourceBindings> _srb;
    std::unique_ptr<QRhiGraphicsPipeline> _pipeline;

    quint32 _instanceCount = 0;

    bool _pipelineDirty = true;
    bool _instancesDirty = false;
};