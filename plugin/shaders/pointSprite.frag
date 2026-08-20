#version 460

layout(location = 0) in vec3 inColor;
layout(location = 0) out vec4 outColor;

void main()
{
    vec2 centeredCoord = gl_PointCoord * 2.0 - 1.0;
    if (dot(centeredCoord, centeredCoord) > 1.0)
    {
        discard;
    }

    outColor = vec4(inColor, 1.0);
}