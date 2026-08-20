#version 460

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;

layout(std140, binding = 0) uniform UniformBuffer {
    mat4 mvp;
    vec4 spriteInfo;
} ubo;

layout(location = 0) out vec3 outColor;

void main()
{
    vec4 clipPosition = ubo.mvp * vec4(inPosition, 1.0);
    gl_Position = clipPosition;
    gl_PointSize = clamp(ubo.spriteInfo.x / max(abs(clipPosition.w), 0.0001), 2.0, 256.0);
    outColor = inColor;
}