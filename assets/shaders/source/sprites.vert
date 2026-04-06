#version 450

layout (location = 0) in vec4 aVertex;
layout (location = 1) in vec2 aPosition;
layout (location = 2) in vec2 aSize;
layout (location = 3) in vec4 aTexCoordsInstance;
layout (location = 4) in uvec4 aColor;
layout (location = 5) in float aLayer;

uniform layout(binding = 0) sla {
    mat4 camProj;
    mat4 camView;
    mat4 spritesProj;
} ubo;

layout (location = 0) out vec2 TexCoords;
layout (location = 1) out vec4 Color;

void main()
{
    const float pixelScale = 3.f;

    gl_Position = ubo.spritesProj * vec4((aPosition * pixelScale) + aVertex.xy * (aSize * pixelScale), 0.f, 1.f);

    TexCoords = mix(aTexCoordsInstance.xy, aTexCoordsInstance.zw, aVertex.zw);
    Color = aColor / 255.f;
} 