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

void MeshLayer::onLoadFinished()
{
    _meshData  = _watcher.future().takeResult();
    _meshDirty = true;
    _loading   = false;

    if (!_meshData->valid)
        _errorString = _meshData->errorString;
    else
        _errorString.clear();

    emit loadingChanged();
    emit errorStringChanged();

    if (_meshData->valid)
        emit dataReady();
}

std::unique_ptr<IRenderable> MeshLayer::createRenderable() const
{
    return std::make_unique<MeshRenderable>();
}
