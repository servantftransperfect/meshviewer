#include "MotionInfo.hpp" 

void MotionInfo::applyTransform()
{
    _rotation = getRotation() * _rotation;
    _center = getUpdatedCenter();
    _distance = _distance + _deltaDistance;
    
    _rotationX = 0.0f;
    _rotationY = 0.0f;
    _planeX = 0.0f;
    _planeY = 0.0f;
    _deltaDistance = 0.0f;
}