#pragma once

struct Vertex {
    float x,  y,  z;   // position
    float nx, ny, nz;  // normal
};

struct ColoredVertex {
    float x, y, z;   // position
    float r, g, b;   // color
};

struct QuadVertex
{
    float x, y;
};