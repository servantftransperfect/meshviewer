#version 460

layout(location = 0) in vec2 vsNdc;

// imageScale.xy: the NDC-space half-extents of the image rectangle.
// The image occupies [-imageScale.x .. +imageScale.x] x [-imageScale.y .. +imageScale.y].
// One of the two components is always 1.0 (the axis that fills the viewport);
// the other is <= 1.0 (the axis that is letterboxed / pillarboxed).
layout(std140, binding = 0) uniform UniformBuffer {
    vec4 imageScale; // .xy used; .zw padding
} ubo;

layout(binding = 1) uniform sampler2D imageSampler;

layout(location = 0) out vec4 outColor;

void main()
{
    // Map NDC position inside [-imageScale .. +imageScale] to UV [0 .. 1].
    // Y is flipped because NDC y+ points up while texture y+ points down.
    vec2 uv;
    uv.x =  vsNdc.x / ubo.imageScale.x * 0.5 + 0.5;
    uv.y = -vsNdc.y / ubo.imageScale.y * 0.5 + 0.5;

    // Fragments outside the image rectangle belong to the letterbox / pillarbox area.
    if (uv.x < 0.0 || uv.x > 1.0 || uv.y < 0.0 || uv.y > 1.0)
        discard;

    outColor = texture(imageSampler, uv);
}
