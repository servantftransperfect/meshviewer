#pragma once

#include "LayerItem.hpp"

#include <aliceVision/image/Image.hpp>
#include <aliceVision/image/pixelTypes.hpp>

#include <QFutureWatcher>
#include <QString>
#include <memory>

struct ImageLoadResult
{
    aliceVision::image::Image<aliceVision::image::RGBAfColor> image;
    QString errorString;
    bool valid = false;
};

/** @brief Layer that asynchronously loads an EXR/image file for full-screen overlay rendering. */
class ImageLayer : public LayerItem
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QString source READ source WRITE setSource NOTIFY sourceChanged)

public:
    explicit ImageLayer(QObject *parent = nullptr);
    ~ImageLayer() override;

    QString source() const { return _source; }
    void setSource(const QString &path);

    // --- Render-thread accessors (call only from IRenderable::sync) ---
    bool imageDirty() const { return _imageDirty; }
    /** @brief Move the loaded image out; resets imageDirty(). Call only from sync(). */
    aliceVision::image::Image<aliceVision::image::RGBAfColor> takeImage();

signals:
    void sourceChanged();

public:
    std::unique_ptr<IRenderable> createRenderable() const override;

private slots:
    void onLoadFinished();

private:
    QString _source;
    bool _imageDirty = false;
    std::unique_ptr<ImageLoadResult> _imageData;
    QFutureWatcher<std::unique_ptr<ImageLoadResult>> _watcher;
};
