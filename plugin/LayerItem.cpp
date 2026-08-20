#include "LayerItem.hpp"

#include "MeshPicker.hpp"

LayerItem::LayerItem(QObject *parent)
    : QObject(parent)
{
}

void LayerItem::setVisible(bool v)
{
    if (v == _visible)
    {
        return;
    }
    
    _visible = v;
    emit visibleChanged();
}

void LayerItem::setPicking(bool picking)
{
    if (picking == _picking)
    {
        return;
    }

    _picking = picking;
    emit pickingChanged();
}

LayerPickResult LayerItem::pick(const Ray &ray) const
{
    Q_UNUSED(ray)
    return {};
}

void LayerItem::applyPickResult(const LayerPickResult &result)
{
    Q_UNUSED(result)
}

void LayerItem::clearPick()
{
}
