#pragma once

#include "LayerItem.hpp"
#include "SfmDataPicker.hpp"

#include <QFutureWatcher>
#include <QMatrix4x4>
#include <QVector3D>

#include <memory>
#include <vector>
#include <optional>

struct SfmDataPointInstance
{
    QVector3D position;
    QVector3D color;
};

struct SfmDataCameraInstance
{
    QMatrix4x4 transform;
    float fovDegrees = 45.0f;
    float aspectRatio = 4.0f / 3.0f;
    float depth = 0.12f;
    float sphereRadius = 0.05f;
};

struct SfmDataRenderData
{
    std::vector<SfmDataPointInstance> points;
    std::vector<SfmDataCameraInstance> cameras;
    QString errorString;
    float pointRadius = 0.01f;
    float cameraScale = 1.0f;
    bool valid = false;
};

class SfmDataLayer : public LayerItem
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QString source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(float pointSize READ pointSize WRITE setPointSize NOTIFY pointSizeChanged)
    Q_PROPERTY(float cameraSize READ cameraSize WRITE setCameraSize NOTIFY cameraSizeChanged)

public:
    explicit SfmDataLayer(QObject *parent = nullptr);
    ~SfmDataLayer() override;

    QString source() const { return _source; }
    void setSource(const QString &path);
    float pointSize() const;
    void setPointSize(float pointSize);
    float cameraSize() const { return _cameraSize; }
    void setCameraSize(float cameraSize);

    const SfmDataRenderData &renderData() const { return *_renderData; }
    bool dataDirty() const { return _dataDirty; }
    void clearDataDirty() { _dataDirty = false; }
    
    std::optional<size_t> selectedCameraIndex() const;

    bool canPick() const override { return true; }
    LayerPickResult pick(const Ray &ray) const override;
    void applyPickResult(const LayerPickResult &result) override;
    void clearPick() override;

signals:
    void sourceChanged();
    void pointSizeChanged();
    void cameraSizeChanged();

public:
    std::unique_ptr<IRenderable> createRenderable() const override;

private slots:
    void onLoadFinished();

private:
    void applyRenderSettings();
    void resetData();
    void rebuildPicker();

    QString _source;
    bool _dataDirty = false;
    std::unique_ptr<SfmDataRenderData> _renderData = std::make_unique<SfmDataRenderData>();
    float _pointSize = 1.0f;
    float _cameraSize = 1.0f;
    SfmDataPicker _picker;
    QFutureWatcher<std::unique_ptr<SfmDataRenderData>> _watcher;
    mutable SfmDataPickHit _lastHit;
};