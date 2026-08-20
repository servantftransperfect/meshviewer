#include "SfmPointRenderable.hpp"

#include <QFile>

namespace {

constexpr int kPointSpriteUniformBufferSize = 80;

QShader loadSfmPointShader(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly))
    {
        qFatal("Cannot open shader %s", qPrintable(path));
    }

    return QShader::fromSerialized(file.readAll());
}

} // namespace

void SfmPointRenderable::setData(std::vector<SfmDataPointInstance> points, float pointRadius)
{
    _points = std::move(points);
    _instancesDirty = true;

    if (!qFuzzyCompare(_pointRadius, pointRadius))
    {
        _pointRadius = pointRadius;
    }
}

void SfmPointRenderable::initialize(QRhi *rhi, QRhiRenderPassDescriptor *rpDesc)
{
    if (_rhi != rhi)
    {
        _rhi = rhi;
        _rpDesc = rpDesc;
        _pipelineDirty = true;
        _instancesDirty = true;
        _pipeline.reset();
        _srb.reset();
        _uniformBuffer.reset();
        _instanceBuffer.reset();
    }

    if (_pipelineDirty)
    {
        buildPipeline();
    }
}

void SfmPointRenderable::prepare(QRhiResourceUpdateBatch *batch, const SceneState &state)
{
    if (_instancesDirty)
    {
        rebuildInstanceBuffer(batch);
    }

    if (_uniformBuffer)
    {
        UniformData uniforms;
        uniforms.mvp = state.viewProjection;
        uniforms.spriteInfo = QVector4D(std::max(_pointRadius * state.projectionScaleY * static_cast<float>(state.viewportHeight), 1.0f),
                                        0.0f,
                                        0.0f,
                                        0.0f);
        batch->updateDynamicBuffer(_uniformBuffer.get(), 0, sizeof(UniformData), &uniforms);
    }
}

void SfmPointRenderable::render(QRhiCommandBuffer *cb, const SceneState &state)
{
    if (!_pipeline)
    {
        return;
    }

    cb->setGraphicsPipeline(_pipeline.get());
    cb->setViewport({ 0, 0, float(state.viewportWidth), float(state.viewportHeight) });
    cb->setShaderResources(_srb.get());

    if (_instanceBuffer && _instanceCount > 0)
    {
        const QRhiCommandBuffer::VertexInput vbs[] = {
        { _instanceBuffer.get(), 0 }
        };
        cb->setVertexInput(0, 1, vbs);
        cb->draw(_instanceCount);
    }
}

void SfmPointRenderable::buildPipeline()
{
    _pipeline.reset();
    _srb.reset();
    _uniformBuffer.reset();

    _uniformBuffer.reset(_rhi->newBuffer(QRhiBuffer::Dynamic, QRhiBuffer::UniformBuffer, kPointSpriteUniformBufferSize));
    _uniformBuffer->create();

    _srb.reset(_rhi->newShaderResourceBindings());
    _srb->setBindings({
        QRhiShaderResourceBinding::uniformBuffer(
            0,
            QRhiShaderResourceBinding::VertexStage,
            _uniformBuffer.get())
    });
    _srb->create();

    QShader vert = loadSfmPointShader(":/shaders/pointSprite.vert.qsb");
    QShader frag = loadSfmPointShader(":/shaders/pointSprite.frag.qsb");

    QRhiVertexInputLayout inputLayout;
    inputLayout.setBindings({
        QRhiVertexInputBinding(sizeof(ColoredVertex))
    });
    inputLayout.setAttributes({
        QRhiVertexInputAttribute(0, 0, QRhiVertexInputAttribute::Float3, 0),
        QRhiVertexInputAttribute(0, 1, QRhiVertexInputAttribute::Float3, sizeof(float) * 3)
    });

    _pipeline.reset(_rhi->newGraphicsPipeline());
    _pipeline->setShaderStages({
        { QRhiShaderStage::Vertex, vert },
        { QRhiShaderStage::Fragment, frag },
    });
    _pipeline->setVertexInputLayout(inputLayout);
    _pipeline->setShaderResourceBindings(_srb.get());
    _pipeline->setRenderPassDescriptor(_rpDesc);
    _pipeline->setTopology(QRhiGraphicsPipeline::Points);
    _pipeline->setDepthTest(true);
    _pipeline->setDepthWrite(true);
    _pipeline->setCullMode(QRhiGraphicsPipeline::None);
    _pipeline->create();

    _pipelineDirty = false;
}

void SfmPointRenderable::rebuildInstanceBuffer(QRhiResourceUpdateBatch *batch)
{
    _instanceBuffer.reset();
    _instanceCount = 0;
    _instancesDirty = false;

    if (_points.empty())
    {
        return;
    }

    std::vector<ColoredVertex> instances;
    instances.reserve(_points.size());
    for (const SfmDataPointInstance &point : _points)
    {
        instances.push_back({
            point.position.x(), point.position.y(), point.position.z(),
            point.color.x(), point.color.y(), point.color.z()
        });
    }

    _instanceCount = static_cast<quint32>(instances.size());
    const quint32 bufSize = _instanceCount * static_cast<quint32>(sizeof(ColoredVertex));

    _instanceBuffer.reset(_rhi->newBuffer(QRhiBuffer::Immutable, QRhiBuffer::VertexBuffer, bufSize));
    _instanceBuffer->create();
    batch->uploadStaticBuffer(_instanceBuffer.get(), instances.data());
}
