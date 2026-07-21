#include "LayerItem.hpp"

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
