#pragma once

#include <QObject> 
#include <QtQml/qqml.h>
#include <QMatrix4x4>

class CameraInfo
{
    Q_GADGET
    QML_STRUCTURED_VALUE
    QML_VALUE_TYPE(class CameraInfo)

    Q_PROPERTY(float fov READ fov WRITE setFov)
    Q_PROPERTY(float nearPlane READ nearPlane WRITE setNearPlane)
    Q_PROPERTY(float farPlane READ farPlane WRITE setFarPlane)
    Q_PROPERTY(int viewportWidth READ viewportWidth WRITE setViewportWidth)
    Q_PROPERTY(int viewportHeight READ viewportHeight WRITE setViewportHeight)    

public:
    float fov() const { return _fov; }
    void setFov(float v) { _fov = v; }

    float nearPlane() const { return _nearPlane; }
    void setNearPlane(float v) { _nearPlane = v; }

    float farPlane() const { return _farPlane; }
    void setFarPlane(float v) { _farPlane = v; }

    int viewportWidth() const { return _viewportWidth; }
    void setViewportWidth(int v) { _viewportWidth = v; }

    int viewportHeight() const { return _viewportHeight; }
    void setViewportHeight(int v) { _viewportHeight = v; }

    bool operator==(const CameraInfo &other) const
    {
        return _fov            == other._fov
            && _nearPlane      == other._nearPlane
            && _farPlane       == other._farPlane
            && _viewportWidth  == other._viewportWidth
            && _viewportHeight == other._viewportHeight;
    }

    QMatrix4x4 getProjectionMatrix() const
    {
        QMatrix4x4 ret = _backendProjectionMatrix;
        
        float aspect = _viewportHeight > 0 ? float(_viewportWidth) / float(_viewportHeight) : 1.0f;
        ret.perspective(_fov, aspect, _nearPlane, _farPlane);

        return ret;
    }

    void setBackendProjectionMatrix(const QMatrix4x4 & bpm)
    {
        _backendProjectionMatrix = bpm;
    }

public:
    QMatrix4x4 _backendProjectionMatrix;

    float _fov = 45.0f;
    float _nearPlane = 0.001f;
    float _farPlane = 100.0f;
    int _viewportWidth = 0;
    int _viewportHeight = 0;
};