#version 450

layout(location = 0) in vec2 aVertex;
layout(location = 1) in vec2 aTexCoords;
layout(location = 2) in vec3 aPosition;
layout(location = 3) in vec3 aSize;

uniform layout(binding = 0) ubo1 {
    mat4 projection;
    mat4 view;
    mat4 model;
} ubo;

layout(location = 0) out vec2 TexCoords;

void main() {
    gl_Position = ubo.projection * ubo.view * vec4(vec3(aVertex, 0.f) * aSize + aPosition, 1.0);

    TexCoords = aTexCoords;
}