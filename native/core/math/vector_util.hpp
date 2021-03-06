//
// Created by Dado on 2019-04-22.
//

#pragma once

#include <core/math/vector3f.h>

struct PickRayData {
    V3f rayNear;
    V3f rayFar;
};

template <typename T>
struct RayPair {
    RayPair() = default;
    RayPair( const T& origin, const T& dir ) : origin(origin), dir(dir) {}

    T origin{0.0f};
    T dir{0.0f};
};

QuadVector3f makeQuadV3f( const Vector3f& p1, const Vector3f& p2, float z2 );