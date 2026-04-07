#version 450

layout(location = 0) in vec2 aVertex;
layout(location = 1) in vec2 aTexCoords;
layout(location = 2) in vec3 aPosition;
layout(location = 3) in vec3 aSize;

uniform layout(binding = 0) sla {
    mat4 camProj;
    mat4 camView;
    mat4 camProjView;
    mat4 spritesProj;
} ubo;

layout(location = 0) out vec2 TexCoords;

void main() {
    //gl_Position = ubo.camProj * ubo.camView * vec4(vec3(aVertex, 0.f) * aSize + aPosition, 1.0);
    gl_Position = ubo.camProjView * vec4(vec3(aVertex, 0.f) * aSize + aPosition, 1.0);

    TexCoords = aTexCoords;
}