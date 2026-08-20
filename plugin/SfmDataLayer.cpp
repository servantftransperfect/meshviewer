#include "SfmDataLayer.hpp"

#include "SfmDataRenderable.hpp"

#include <aliceVision/sfmData/Landmark.hpp>
#include <aliceVision/sfmData/SfMData.hpp>
#include <aliceVision/sfmDataIO/sfmDataIO.hpp>

#include <QtConcurrent/QtConcurrent>
#include <QUrl>

#include <algorithm>
#include <cmath>
#include <limits>

namespace {

constexpr float kMinPointSize = 0.00005f;
constexpr float kMinCameraSize = 0.0001f;

std::unique_ptr<SfmDataRenderData> loadSfmDataFile(const QString &path)
{
    auto result = std::make_unique<SfmDataRenderData>();

    aliceVision::sfmData::SfMData sfmData;
    if (!aliceVision::sfmDataIO::load(sfmData,
                                      path.toStdString(),
                                      static_cast<aliceVision::sfmDataIO::ESfMData>(aliceVision::sfmDataIO::VIEWS | 
                                                            aliceVision::sfmDataIO::EXTRINSICS | 
                                                            aliceVision::sfmDataIO::INTRINSICS | 
                                                            aliceVision::sfmDataIO::STRUCTURE)))
    {
        result->errorString = QStringLiteral("Failed to load sfmData file: %1").arg(path);
        return result;
    }

    result->points.reserve(sfmData.getLandmarks().size());
    for (const auto &[trackId, landmark] : sfmData.getLandmarks())
    {
        Q_UNUSED(trackId)

        const auto &position = landmark.getX();
        const auto &rgb = landmark.getRgb();

        result->points.push_back({
            QVector3D(position.x(), -position.y(), -position.z()),
            QVector3D(rgb.r() / 255.0f, rgb.g() / 255.0f, rgb.b() / 255.0f)
        });
    }

    // Camera extraction is intentionally left to the caller. Populate
    // result->cameras here when AliceVision camera/view data is available.
    for (const auto [idView, view] : sfmData.getViews().valueRange())
    {
        if (!sfmData.isPoseAndIntrinsicDefined(view))
        {
            continue;
        }

        const aliceVision::camera::IntrinsicBase & camera = sfmData.getIntrinsic(view.getIntrinsicId());

        Eigen::Matrix4d c_T_w = sfmData.getPose(view).getTransform().inverse().getHomogeneous();
        Eigen::Matrix4d transform = Eigen::Matrix4d::Identity();
        transform(1, 1) = -1;
        transform(2, 2) = -1;
        Eigen::Matrix4d c_T_w_3d = (transform * c_T_w * transform);


        SfmDataCameraInstance camdata;
        camdata.fovDegrees = camera.getHorizontalFov() * 180.0 / M_PI;
        camdata.aspectRatio = static_cast<float>(camera.w()) / static_cast<float>(camera.h());
        
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                camdata.transform(i, j) = c_T_w_3d(i, j);
            }
        }

        result->cameras.push_back(camdata);
    }

    result->valid = true;
   

    return result;
}

} // namespace

SfmDataLayer::SfmDataLayer(QObject *parent)
    : LayerItem(parent)
{
    connect(&_watcher, &QFutureWatcher<std::unique_ptr<SfmDataRenderData>>::finished,
            this, &SfmDataLayer::onLoadFinished);
}

SfmDataLayer::~SfmDataLayer()
{
    if (_watcher.isRunning())
    {
        _watcher.waitForFinished();
    }
}

void SfmDataLayer::setSource(const QString &path)
{
    if (_source == path)
    {
        return;
    }

    _source = path;
    emit sourceChanged();

    if (path.isEmpty())
    {
        resetData();
        return;
    }

    QString filePath = path;
    if (filePath.startsWith("file://"))
    {
        filePath = QUrl(filePath).toLocalFile();
    }

    _loading = true;
    _errorString.clear();
    emit loadingChanged();
    emit errorStringChanged();

    _watcher.setFuture(
        QtConcurrent::run([filePath]() {
            return loadSfmDataFile(filePath);
        })
    );
}

float SfmDataLayer::pointSize() const
{
    return _pointSize;
}

void SfmDataLayer::setPointSize(float pointSize)
{
    const float clampedPointSize = std::max(pointSize, kMinPointSize);
    if (std::abs(_pointSize - clampedPointSize) <= kMinPointSize)
    {
        return;
    }

    _pointSize = clampedPointSize;
    applyRenderSettings();

    emit pointSizeChanged();
}

void SfmDataLayer::setCameraSize(float cameraSize)
{
    const float clampedCameraSize = std::max(cameraSize, kMinCameraSize);
    if (std::abs(_cameraSize - clampedCameraSize) <= kMinCameraSize)
    {
        return;
    }

    _cameraSize = clampedCameraSize;
    applyRenderSettings();

    emit cameraSizeChanged();
}

void SfmDataLayer::onLoadFinished()
{
    _renderData = _watcher.future().takeResult();
    applyRenderSettings();
    _dataDirty = true;
    _loading = false;

    if (!_renderData->valid)
    {
        _errorString = _renderData->errorString;
    }
    else
    {
        _errorString.clear();
        rebuildPicker();
    }

    emit loadingChanged();
    emit errorStringChanged();
    emit dataReady();
}

std::unique_ptr<IRenderable> SfmDataLayer::createRenderable() const
{
    return std::make_unique<SfmDataRenderable>();
}

void SfmDataLayer::resetData()
{
    _renderData = std::make_unique<SfmDataRenderData>();
    _renderData->valid = true;
    applyRenderSettings();
    rebuildPicker();
    _dataDirty = true;
    _loading = false;
    _errorString.clear();
    emit loadingChanged();
    emit errorStringChanged();
    emit dataReady();
}

LayerPickResult SfmDataLayer::pick(const Ray &ray) const
{
    if (!_picker.isReady())
    {
        return {};
    }

    const SfmDataPickHit hit = _picker.pick(ray);
    if (!hit.hit)
    {
        return {};
    }

    _lastHit = hit;

    return {
        true,
        hit.distance,
        hit.worldPoint
    };
}

std::optional<size_t> SfmDataLayer::selectedCameraIndex() const
{
    const SfmDataPickHit &selectedObject = _picker.selectedObject();
    if (!selectedObject.hit || selectedObject.kind != SfmDataPickHit::Kind::Camera)
    {
        return std::nullopt;
    }

    return selectedObject.index;
}

void SfmDataLayer::rebuildPicker()
{
    if (_renderData && _renderData->valid)
    {
        _picker.build(_renderData->points,
                      _renderData->pointRadius,
                      _renderData->cameras,
                      _renderData->cameraScale);
        return;
    }

    _picker.build({}, 0.0f, {}, 1.0f);
}

void SfmDataLayer::applyRenderSettings()
{
    if (!_renderData)
    {
        return;
    }

    _renderData->pointRadius = _pointSize;

    _renderData->cameraScale = std::max(_cameraSize, kMinCameraSize);
    _dataDirty = true;
    rebuildPicker();

    if (!_loading)
    {
        emit dataReady();
    }
}

void SfmDataLayer::applyPickResult(const LayerPickResult &result)
{
    Q_UNUSED(result)
    _picker.setSelectedObject(_lastHit);
}

void SfmDataLayer::clearPick() 
{
    _picker.clearSelectedObject();
    _lastHit.kind = SfmDataPickHit::Kind::None;
}