#pragma once

#include <QObject> 
#include <QtQml/qqml.h>
#include <QMatrix4x4>
#include <QVector3D>

#include <QDebug>

class MotionInfo
{
    Q_GADGET
    QML_STRUCTURED_VALUE
    QML_VALUE_TYPE(class MotionInfo)

    Q_PROPERTY(float rotationX READ rotationX WRITE setRotationX)
    Q_PROPERTY(float rotationY READ rotationY WRITE setRotationY)
    Q_PROPERTY(float planeX READ planeX WRITE setPlaneX)
    Q_PROPERTY(float planeY READ planeY WRITE setPlaneY)
    Q_PROPERTY(float distance READ distance WRITE setDistance)

public:
    Q_INVOKABLE void applyTransform();

public:
    float rotationX() const { return _rotationX; }
    float rotationY() const { return _rotationY; }
    float planeX() const { return _planeX; }
    float planeY() const { return _planeY; }
    float distance() const { return _deltaDistance; }
    
    void setRotationX(float v)
    {
        _rotationX = v;
    }

    void setRotationY(float v)
    {
        _rotationY = v;
    }

    void setPlaneX(float v)
    {
        _planeX = v;
    }

    void setPlaneY(float v)
    {
        _planeY = v;
    }

    void setDistance(float v)
    {
        _deltaDistance = v;
    }

    void setCenter(const QVector3D & center)
    {
        _center = center;
    }

    bool operator==(const MotionInfo &other) const
    {
        return _rotationX == other._rotationX 
            && _rotationY == other._rotationY 
            && _planeX == other._planeX 
            && _planeY == other._planeY 
            && _deltaDistance == other._deltaDistance;
    }

    QMatrix4x4 getRotation() const 
    {
        QMatrix4x4 ret;

        ret.rotate(_rotationX, 1.0, 0.0, 0.0);
        ret.rotate(_rotationY, 0.0, 1.0, 0.0);
        
        return ret;
    }

    QVector3D getUpdatedCenter() const 
    {
        QVector3D planeMotion;

        planeMotion[0] = _planeX;
        planeMotion[1] = -_planeY;
        planeMotion[2] = 0;

        return _rotation.transposed().mapVector(planeMotion) + _center;
    }

    QMatrix4x4 getMatrix() const 
    {
        QMatrix4x4 ret;
        QMatrix4x4 A;

        QVector3D upCenter = getUpdatedCenter();

        A(0, 3) = -upCenter[0];
        A(1, 3) = -upCenter[1];
        A(2, 3) = -upCenter[2];
        
        ret.rotate(_rotationX, 1.0, 0.0, 0.0);
        ret.rotate(_rotationY, 0.0, 1.0, 0.0);

        ret(2, 3) = - (_distance + _deltaDistance);

        return ret * _rotation * A;
    }

public:
    float _rotationX = 0.0f;
    float _rotationY = 0.0f;
    float _planeX = 0.0f;
    float _planeY = 0.0f;
    float _deltaDistance = 0.0f;
    
    float _distance = 1.0f;
    QVector3D _center;
    QMatrix4x4 _rotation;
};