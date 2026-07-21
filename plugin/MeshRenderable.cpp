#include "MeshRenderable.hpp"
#include "MeshLayer.hpp"
#include "MeshData.hpp"

#include <QFile>

static QShader loadMeshShader(const QString &path)
{
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly))
    {
        qFatal("Cannot open shader %s", qPrintable(path));
    }

    return QShader::fromSerialized(f.readAll());
}

void MeshRenderable::sync(LayerItem *layer, SceneView * /*view*/)
{
    MeshLayer *meshLayer = static_cast<MeshLayer *>(layer);
    if (!meshLayer->meshDirty())
        return;

    const MeshData &data = meshLayer->meshData();

    _vertices.clear();
    _indices.clear();

    quint32 vertexOffset = 0;
    for (const SubMesh &sub : data.subMeshes)
    {
        _vertices += sub.vertices;
        for (quint32 idx : sub.indices)
            _indices.append(idx + vertexOffset);
        vertexOffset += sub.vertices.size();
    }

    const QVector3D meshCenter = { data.centerX(), data.centerY(), data.centerZ() };

    meshLayer->clearMeshDirty();
    _buffersDirty = true;
}

void MeshRenderable::initialize(QRhi *rhi, QRhiRenderPassDescriptor *rpDesc)
{
    if (_rhi != rhi)
    {
        _rhi = rhi;
        _rpDesc = rpDesc;
        _pipelineDirty = true;
        _buffersDirty = true;
        _pipeline.reset();
        _srb.reset();
        _uniformBuffer.reset();
        _vertexBuffer.reset();
        _indexBuffer.reset();
    }

    if (_pipelineDirty)
    {
        buildPipeline();
    }
}

void MeshRenderable::prepare(QRhiResourceUpdateBatch *batch, const SceneState &state)
{
    if (_buffersDirty)
    {
        rebuildBuffers(batch);
    }

    if (_uniformBuffer)
    {
        QMatrix4x4 mvp = state.viewProjection * _modelMatrix;
        float uboData[32];
        memcpy(uboData, mvp.constData(), 64);
        memcpy(uboData + 16, state.normalMatrix.constData(), 64);
        batch->updateDynamicBuffer(_uniformBuffer.get(), 0, 128, uboData);
    }
}

void MeshRenderable::render(QRhiCommandBuffer *cb, const SceneState &state)
{
    if (!_pipeline || !_vertexBuffer || !_indexBuffer)
    {
        return;
    }

    cb->setGraphicsPipeline(_pipeline.get());
    cb->setViewport({ 0, 0, float(state.viewportWidth), float(state.viewportHeight) });
    cb->setShaderResources(_srb.get());

    const QRhiCommandBuffer::VertexInput vb(_vertexBuffer.get(), 0);
    cb->setVertexInput(0, 1, &vb, _indexBuffer.get(), 0, QRhiCommandBuffer::IndexUInt32);
    cb->drawIndexed(static_cast<quint32>(_indices.size()));
}

void MeshRenderable::rebuildBuffers(QRhiResourceUpdateBatch *batch)
{
    _vertexBuffer.reset();
    _indexBuffer.reset();
    _buffersDirty = false;

    if (_vertices.isEmpty() || _indices.isEmpty())
    {
        return;
    }

    const quint32 vbSize = static_cast<quint32>(_vertices.size()) * sizeof(Vertex);
    const quint32 ibSize = static_cast<quint32>(_indices.size()) * sizeof(quint32);

    _vertexBuffer.reset(_rhi->newBuffer(QRhiBuffer::Immutable, QRhiBuffer::VertexBuffer, vbSize));
    _vertexBuffer->create();

    _indexBuffer.reset(_rhi->newBuffer(QRhiBuffer::Immutable, QRhiBuffer::IndexBuffer, ibSize));
    _indexBuffer->create();

    batch->uploadStaticBuffer(_vertexBuffer.get(), _vertices.constData());
    batch->uploadStaticBuffer(_indexBuffer.get(), _indices.constData());
}

void MeshRenderable::buildPipeline()
{
    _pipeline.reset();
    _srb.reset();
    _uniformBuffer.reset();

    // Uniform buffer: MVP + normalMatrix (2 × mat4 = 128 bytes)
    _uniformBuffer.reset(_rhi->newBuffer(QRhiBuffer::Dynamic, QRhiBuffer::UniformBuffer, 128));
    _uniformBuffer->create();

    _srb.reset(_rhi->newShaderResourceBindings());
    _srb->setBindings({
        QRhiShaderResourceBinding::uniformBuffer(
            0,
            QRhiShaderResourceBinding::VertexStage |
            QRhiShaderResourceBinding::FragmentStage,
            _uniformBuffer.get())
    });
    _srb->create();

    QShader vert = loadMeshShader(":/shaders/mesh.vert.qsb");
    QShader frag = loadMeshShader(":/shaders/mesh.frag.qsb");

    // Vertex layout: position (vec3) + normal (vec3)
    QRhiVertexInputLayout inputLayout;
    inputLayout.setBindings({ QRhiVertexInputBinding(sizeof(Vertex)) });
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
    _pipeline->setDepthTest(true);
    _pipeline->setDepthWrite(true);
    _pipeline->setCullMode(QRhiGraphicsPipeline::Back);
    _pipeline->create();

    _pipelineDirty = false;
}
