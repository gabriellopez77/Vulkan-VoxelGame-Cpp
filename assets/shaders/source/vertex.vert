#version 450

layout(location = 0) in vec2 aVertex;
layout(location = 1) in vec2 aTexCoords;
layout(location = 2) in vec3 aPosition;
layout(location = 3) in vec2 aSize;

uniform layout(binding = 0) ubo1 {
    mat4 projection;
    mat4 view;
} ubo;

uniform layout(push_constant) ubo12 {
    mat4 model;
} ubo2;

layout(location = 0) out vec2 TexCoords;

void main() {
    gl_Position = ubo.projection * ubo.view * ubo2.model* vec4(aVertex, 0.f, 1.0);

    TexCoords = aTexCoords;
}