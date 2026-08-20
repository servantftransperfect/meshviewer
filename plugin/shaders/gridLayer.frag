#version 460

layout(std140, binding = 0) uniform GridUniformBuffer {
    mat4 inverseViewProjection;
    vec4 viewportSize;
    vec4 spacingAndWidth;
    vec4 minorGridParams;
} ubo;

layout(location = 0) out vec4 outColor;

vec3 unprojectPoint(vec2 ndc, float ndcZ)
{
    vec4 worldPositionH = ubo.inverseViewProjection * vec4(ndc, ndcZ, 1.0);
    return worldPositionH.xyz / max(abs(worldPositionH.w), 1e-6);
}

float gridLineAlpha(vec2 worldCoord, float spacing, float lineWidthPixels)
{
    vec2 gridCoord = worldCoord / spacing;
    vec2 gridDeriv = max(fwidth(gridCoord), vec2(1e-4));
    vec2 lineDist = abs(fract(gridCoord - 0.5) - 0.5);
    vec2 lineAlpha = 1.0 - smoothstep(vec2(0.0), gridDeriv * lineWidthPixels, lineDist);
    return max(lineAlpha.x, lineAlpha.y);
}

void main()
{
    vec2 ndc = vec2(
        (gl_FragCoord.x / max(ubo.viewportSize.x, 1.0)) * 2.0 - 1.0,
        (gl_FragCoord.y / max(ubo.viewportSize.y, 1.0)) * 2.0 - 1.0);

    vec3 nearPoint = unprojectPoint(ndc, -1.0);
    vec3 farPoint = unprojectPoint(ndc, 1.0);
    vec3 rayDirection = farPoint - nearPoint;

    float denom = rayDirection.y;
    bool hasPlaneHit = abs(denom) > 1e-6;
    float t = hasPlaneHit ? (-nearPoint.y / denom) : 0.0;
    vec3 planeHit = nearPoint + rayDirection * t;

    if (!hasPlaneHit || t < 0.0)
    {
        discard;
    }

    float majorSpacing = max(ubo.spacingAndWidth.x, 1e-4);
    float minorSpacing = max(ubo.spacingAndWidth.y, 1e-4);
    float majorLineWidth = max(ubo.spacingAndWidth.z, 0.25);
    float minorLineWidth = max(ubo.spacingAndWidth.w, 0.25);
    float minorOpacity = clamp(ubo.minorGridParams.x, 0.0, 1.0);
    float minorFadeStartPixels = max(ubo.minorGridParams.y, 0.0);
    float minorFadeEndPixels = max(ubo.minorGridParams.z, minorFadeStartPixels + 1e-4);

    float worldUnitsPerPixel = max(max(fwidth(planeHit.x), fwidth(planeHit.z)), 1e-5);
    float minorPixels = minorSpacing / worldUnitsPerPixel;

    float majorGrid = gridLineAlpha(planeHit.xz, majorSpacing, majorLineWidth);
    float minorGrid = gridLineAlpha(planeHit.xz, minorSpacing, minorLineWidth) * minorOpacity * smoothstep(minorFadeStartPixels, minorFadeEndPixels, minorPixels);

    float axisX = 1.0 - smoothstep(0.0, max(fwidth(planeHit.x) * 1.5, 1e-4), abs(planeHit.x));
    float axisZ = 1.0 - smoothstep(0.0, max(fwidth(planeHit.z) * 1.5, 1e-4), abs(planeHit.z));

    vec3 baseColor = vec3(0.12, 0.18, 0.24);
    vec3 minorColor = vec3(0.25, 0.33, 0.40);
    vec3 majorColor = vec3(0.34, 0.42, 0.50);
    vec3 axisXColor = vec3(0.85, 0.25, 0.20);
    vec3 axisZColor = vec3(0.20, 0.55, 0.85);

    vec3 color = mix(baseColor, minorColor, minorGrid);
    color = mix(color, majorColor, majorGrid);
    color = mix(color, axisXColor, axisX);
    color = mix(color, axisZColor, axisZ);

    outColor = vec4(color, 1.0);
}
