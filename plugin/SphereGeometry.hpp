#pragma once

#include <QVector>
#include <cmath>
#include "vertex.hpp"

/**
 * @brief Builds a UV sphere mesh into @p verts and @p indices.
 *
 * The sphere is centred at the origin. Instance offsets (world-space centres)
 * are applied externally in the vertex shader via a per-instance buffer.
 *
 * @param verts   Output vertex array (cleared on entry).
 * @param indices Output index array (cleared on entry).
 * @param radius  Sphere radius.
 * @param r,g,b   Uniform RGB colour applied to all vertices.
 * @param stacks  Number of horizontal rings (latitude subdivisions).
 * @param slices  Number of vertical columns (longitude subdivisions).
 */
inline void buildSphereMesh(
    QVector<ColoredVertex> &verts,
    QVector<quint32> &indices,
    float radius = 0.05f,
    float r = 1.0f, float g = 1.0f, float b = 0.0f,
    int stacks = 12, int slices = 18)
{
    verts.clear();
    indices.clear();

    auto addVert = [&](float x, float y, float z) -> quint32 {
        quint32 idx = static_cast<quint32>(verts.size());
        verts.append({x, y, z, r, g, b});
        return idx;
    };

    // Top pole
    quint32 topPole = addVert(0.f, radius, 0.f);

    // Intermediate rings (phi from just below top to just above bottom)
    for (int i = 1; i < stacks; ++i) {
        const float phi = float(M_PI * i / stacks);
        const float y   = radius * std::cos(phi);
        const float rr  = radius * std::sin(phi);
        for (int j = 0; j < slices; ++j) {
            const float theta = float(2.0 * M_PI * j / slices);
            addVert(rr * std::cos(theta), y, rr * std::sin(theta));
        }
    }

    // Bottom pole
    quint32 botPole = addVert(0.f, -radius, 0.f);

    // Top cap: fan around top pole
    for (int j = 0; j < slices; ++j) {
        const quint32 cur  = 1 + j;
        const quint32 next = 1 + (j + 1) % slices;
        indices.append(topPole);
        indices.append(cur);
        indices.append(next);
    }

    // Middle quads (stacks-2 bands between adjacent rings)
    for (int i = 0; i < stacks - 2; ++i) {
        const quint32 base = 1 + static_cast<quint32>(i * slices);
        for (int j = 0; j < slices; ++j) {
            const quint32 next = static_cast<quint32>((j + 1) % slices);
            const quint32 a = base + j;                     // ring i,   col j
            const quint32 b = base + next;                  // ring i,   col j+1
            const quint32 c = base + slices + j;            // ring i+1, col j
            const quint32 d = base + slices + next;         // ring i+1, col j+1
            indices.append(a); indices.append(b); indices.append(c);
            indices.append(b); indices.append(d); indices.append(c);
        }
    }

    // Bottom cap: fan around bottom pole
    const quint32 lastRingBase = 1 + static_cast<quint32>((stacks - 2) * slices);
    for (int j = 0; j < slices; ++j) {
        const quint32 cur  = lastRingBase + j;
        const quint32 next = lastRingBase + (j + 1) % slices;
        indices.append(botPole);
        indices.append(next);
        indices.append(cur);
    }
}
