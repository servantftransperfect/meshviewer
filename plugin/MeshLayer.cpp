#include "MeshLayer.hpp"
#include "MeshLoader.hpp"
#include "MeshRenderable.hpp"

#include <QtConcurrent/QtConcurrent>
#include <QUrl>

MeshLayer::MeshLayer(QObject *parent)
    : LayerItem(parent)
{
    connect(&_watcher, &QFutureWatcher<std::unique_ptr<MeshData>>::finished,
            this, &MeshLayer::onLoadFinished);
}

MeshLayer::~MeshLayer()
{
    if (_watcher.isRunning())
        _watcher.waitForFinished();
}

void MeshLayer::setSource(const QString &path)
{
    if (_source == path)
        return;

    _source = path;
    emit sourceChanged();

    if (path.isEmpty())
        return;

    QString filePath = path;
    if (filePath.startsWith("file://"))
    {
        filePath = QUrl(filePath).toLocalFile();
    }

    _loading = true;
    emit loadingChanged();

    _watcher.setFuture(
        QtConcurrent::run([filePath]() {
            return MeshLoader::load(filePath);
        })
    );
}

void MeshLayer::setSelection(const QVector3D &selection)
{
    if (_selection == selection)
    {
        return;
    }

    _selection = selection;
    _selectionDirty = true;

    emit selectionChanged();
    emit dataReady();
}

void MeshLayer::onLoadFinished()
{
    _meshData  = _watcher.future().takeResult();
    _meshDirty = true;
    _loading   = false;

    if (!_meshData->valid)
        _errorString = _meshData->errorString;
    else
    {
        _picker.buildBVH(*_meshData);
        _errorString.clear();
    }

    emit loadingChanged();
    emit errorStringChanged();

    if (_meshData->valid)
        emit dataReady();
}

std::unique_ptr<IRenderable> MeshLayer::createRenderable() const
{
    return std::make_unique<MeshRenderable>();
}

LayerPickResult MeshLayer::pick(const Ray &ray) const
{
    if (!_meshData || !_meshData->valid || !_picker.isReady())
    {
        return {};
    }

    const HitResult hit = _picker.pick(ray);
    if (!hit.hit)
    {
        return {};
    }

    const QVector3D direction = ray.direction.normalized();
    return {
        true,
        hit.distance,
        ray.origin + direction * hit.distance
    };
}

void MeshLayer::applyPickResult(const LayerPickResult &result)
{
    if (!result.hit)
    {
        return;
    }

    setSelection(result.worldPoint);
}

void MeshLayer::clearPick()
{

}