#include "SceneView.hpp"
#include "SceneRenderer.hpp"

SceneView::SceneView(QQuickItem *parent)
    : QQuickRhiItem(parent)
{
    setFlag(QQuickItem::ItemHasContents, true);
}

SceneView::~SceneView() = default;

QQuickRhiItemRenderer *SceneView::createRenderer()
{
    return new SceneRenderer();
}

// ── Layers ──────────────────────────────────────────────────────────────────

QQmlListProperty<LayerItem> SceneView::layers()
{
    return QQmlListProperty<LayerItem>(this, nullptr,
                                       &SceneView::layersAppend,
                                       &SceneView::layersCount,
                                       &SceneView::layersAt,
                                       &SceneView::layersClear);
}

void SceneView::layersAppend(QQmlListProperty<LayerItem> *list, LayerItem *item)
{
    auto *self = static_cast<SceneView *>(list->object);
    self->_layers.append(item);
    self->rebuildLayerConnections();
    emit self->layersChanged();
    self->update();
}

qsizetype SceneView::layersCount(QQmlListProperty<LayerItem> *list)
{
    return static_cast<SceneView *>(list->object)->_layers.count();
}

LayerItem *SceneView::layersAt(QQmlListProperty<LayerItem> *list, qsizetype index)
{
    return static_cast<SceneView *>(list->object)->_layers.at(index);
}

void SceneView::layersClear(QQmlListProperty<LayerItem> *list)
{
    auto *self = static_cast<SceneView *>(list->object);
    self->_layers.clear();
    self->rebuildLayerConnections();
    emit self->layersChanged();
    self->update();
}

void SceneView::rebuildLayerConnections()
{
    for (auto &conn : _layerConnections)
        QObject::disconnect(conn);
    _layerConnections.clear();

    for (LayerItem *layer : _layers)
    {
        _layerConnections += connect(layer, &LayerItem::visibleChanged,
                                     this, [this]() { update(); });
        _layerConnections += connect(layer, &LayerItem::loadingChanged,
                                     this, &SceneView::recomputeAggregateState);
        _layerConnections += connect(layer, &LayerItem::errorStringChanged,
                                     this, &SceneView::recomputeAggregateState);
        _layerConnections += connect(layer, &LayerItem::dataReady,
                                     this, [this]() {
                                         update();
                                         polish();
                                     });
    }
}

void SceneView::recomputeAggregateState()
{
    bool anyLoading = false;
    QString firstError;

    for (LayerItem *layer : _layers)
    {
        if (layer->loading()) anyLoading = true;
        if (firstError.isEmpty()) firstError = layer->errorString();
    }

    if (anyLoading != _loading)
    {
        _loading = anyLoading;
        emit loadingChanged();
    }
    if (firstError != _errorString)
    {
        _errorString = firstError;
        emit errorStringChanged();
    }
}

// ── Camera / motion ──────────────────────────────────────────────────────────

void SceneView::setMotionInfo(const MotionInfo &mi)
{
    if (mi == _motionInfo)
        return;
    _motionInfo = mi;
    emit motionInfoChanged();
    update();
}

void SceneView::setCameraInfo(const CameraInfo &ci)
{
    if (ci == _cameraInfo)
        return;
    _cameraInfo = ci;
    emit cameraInfoChanged();
    update();
}

