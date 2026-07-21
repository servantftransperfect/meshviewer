#pragma once

#include "LayerItem.hpp"
#include "MeshData.hpp"

#include <QFutureWatcher>
#include <QString>
#include <memory>

/** @brief Layer that asynchronously loads and exposes an OBJ/PLY mesh file. */
class MeshLayer : public LayerItem
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QString source READ source WRITE setSource NOTIFY sourceChanged)

public:
    explicit MeshLayer(QObject *parent = nullptr);
    ~MeshLayer() override;

    QString source() const { return _source; }
    void setSource(const QString &path);

    // --- Render-thread accessors (call only from IRenderable::sync) ---
    const MeshData &meshData() const { return *_meshData; }
    bool meshDirty() const { return _meshDirty; }
    void clearMeshDirty() { _meshDirty = false; }

signals:
    void sourceChanged();

public:
    std::unique_ptr<IRenderable> createRenderable() const override;

private slots:
    void onLoadFinished();

private:
    QString _source;
    bool _meshDirty = false;
    std::unique_ptr<MeshData> _meshData = std::make_unique<MeshData>();
    QFutureWatcher<std::unique_ptr<MeshData>> _watcher;
};
