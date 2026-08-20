#pragma once

#include "LayerItem.hpp"
#include "MeshData.hpp"
#include "MeshPicker.hpp"

#include <QFutureWatcher>
#include <QString>
#include <QVector3D>
#include <memory>

/** @brief Layer that asynchronously loads and exposes an OBJ/PLY mesh file. */
class MeshLayer : public LayerItem
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QString source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(QVector3D selection READ selection WRITE setSelection NOTIFY selectionChanged)

public:
    explicit MeshLayer(QObject *parent = nullptr);
    ~MeshLayer() override;

    QString source() const { return _source; }
    void setSource(const QString &path);

    QVector3D selection() const { return _selection; }
    void setSelection(const QVector3D &selection);

    // --- Render-thread accessors (call only from IRenderable::sync) ---
    const MeshData &meshData() const { return *_meshData; }
    bool meshDirty() const { return _meshDirty; }
    void clearMeshDirty() { _meshDirty = false; }
    bool selectionDirty() const { return _selectionDirty; }
    void clearSelectionDirty() { _selectionDirty = false; }

    bool canPick() const override { return true; }
    LayerPickResult pick(const Ray &ray) const override;
    void applyPickResult(const LayerPickResult &result) override;
    void clearPick() override;

signals:
    void sourceChanged();
    void selectionChanged();

public:
    std::unique_ptr<IRenderable> createRenderable() const override;

private slots:
    void onLoadFinished();

private:
    QString _source;
    QVector3D _selection;
    bool _meshDirty = false;
    bool _selectionDirty = true;
    std::unique_ptr<MeshData> _meshData = std::make_unique<MeshData>();
    MeshPicker _picker;
    QFutureWatcher<std::unique_ptr<MeshData>> _watcher;
};
