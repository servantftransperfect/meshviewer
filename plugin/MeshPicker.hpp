#pragma once

#include "MeshData.hpp"

#include <embree4/rtcore.h>
#include <QMatrix4x4>
#include <QVector2D>
#include <QVector3D>
#include <memory>

struct Ray
{
    QVector3D origin;
    QVector3D direction;  // unit vector
};

struct HitResult
{
    bool        hit      = false;
    float       distance = 0.0f;
    unsigned int geomID  = 0;  // sub-mesh index
    unsigned int primID  = 0;  // triangle index within that sub-mesh
    QVector3D   normal;        // geometric normal, world space
};

class MeshPicker
{
public:
    MeshPicker() = default;
    ~MeshPicker();

    void buildBVH(const MeshData &meshData);

    bool isReady() const { return _scene != nullptr; }

    HitResult pick(const Ray &ray) const;

    static Ray unprojectRay(const QMatrix4x4 &projection,
                            const QMatrix4x4 &transform,
                            const QVector2D  &mousePos,
                            float viewportWidth,
                            float viewportHeight);

private:
    void releaseScene();

    const MeshData *_meshData = nullptr;  // non-owning; Embree borrows its buffers

    RTCDevice _device = nullptr;
    RTCScene  _scene  = nullptr;
};
