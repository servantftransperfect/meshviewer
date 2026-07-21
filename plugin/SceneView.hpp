#pragma once

#include <QQuickRhiItem>
#include <QQmlListProperty>
#include <QMetaObject>
#include <QVector2D>
#include <QVector3D>
#include <memory>

#include <qqml.h>

#include "LayerItem.hpp"
#include "MotionInfo.hpp"
#include "CameraInfo.hpp"
#include "MeshPicker.hpp"

class SceneRenderer;

/**
 * @brief Top-level QML item hosting a declarative list of rendering layers.
 *
 * Camera/motion state and ray-pick logic live here. Individual layers declare
 * what to render; SceneRenderer builds the corresponding IRenderable pipeline.
 */
class SceneView : public QQuickRhiItem
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QQmlListProperty<LayerItem> layers READ layers NOTIFY layersChanged)
    Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)
    Q_PROPERTY(QString errorString READ errorString NOTIFY errorStringChanged)

    Q_PROPERTY(MotionInfo motionInfo READ getMotionInfo WRITE setMotionInfo NOTIFY motionInfoChanged)
    Q_PROPERTY(CameraInfo cameraInfo READ getCameraInfo WRITE setCameraInfo NOTIFY cameraInfoChanged)

public:
    explicit SceneView(QQuickItem *parent = nullptr);
    ~SceneView() override;

    QQuickRhiItemRenderer *createRenderer() override;

    // --- Layers ---
    QQmlListProperty<LayerItem> layers();
    const QList<LayerItem *> &layerList() const { return _layers; }

    // --- Aggregate loading state ---
    bool loading() const { return _loading; }
    QString errorString() const { return _errorString; }

    // --- Camera / motion ---
    MotionInfo &getMotionInfo() { return _motionInfo; }
    MotionInfo getMotionInfo() const { return _motionInfo; }
    void setMotionInfo(const MotionInfo &mi);

    CameraInfo &getCameraInfo() { return _cameraInfo; }
    CameraInfo getCameraInfo() const { return _cameraInfo; }
    void setCameraInfo(const CameraInfo &ci);

signals:
    void layersChanged();
    void loadingChanged();
    void errorStringChanged();
    void motionInfoChanged();
    void cameraInfoChanged();

private slots:
    void recomputeAggregateState();

private:
    // QQmlListProperty callbacks
    static void layersAppend(QQmlListProperty<LayerItem> *list, LayerItem *item);
    static qsizetype layersCount(QQmlListProperty<LayerItem> *list);
    static LayerItem *layersAt(QQmlListProperty<LayerItem> *list, qsizetype index);
    static void layersClear(QQmlListProperty<LayerItem> *list);

    void rebuildLayerConnections();

    QList<LayerItem *> _layers;
    QList<QMetaObject::Connection> _layerConnections;

    bool _loading = false;
    QString _errorString;

    MotionInfo _motionInfo;
    CameraInfo _cameraInfo;
};
