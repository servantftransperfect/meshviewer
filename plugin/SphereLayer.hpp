#pragma once

#include "LayerItem.hpp"
#include "SphereRenderable.hpp"

/** @brief Layer that renders GPU-instanced spheres at each picked point. */
class SphereLayer : public LayerItem
{
    Q_OBJECT
    QML_ELEMENT

public:
    explicit SphereLayer(QObject *parent = nullptr) : LayerItem(parent) {}

    std::unique_ptr<IRenderable> createRenderable() const override
    {
        return std::make_unique<SphereRenderable>();
    }
};
