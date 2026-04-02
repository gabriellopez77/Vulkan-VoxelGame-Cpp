#version 450

layout(location = 0) in vec2 aVertex;
layout(location = 1) in vec2 aTexCoords;
layout(location = 2) in vec3 aPosition;
layout(location = 3) in vec2 aSize;

layout(binding = 0) uniform sla {
    mat4 projection;
    mat4 view;
} ubo;

layout(location = 0) out vec2 TexCoords;

void main() {
    gl_Position = ubo.projection * ubo.view * vec4(vec3(aVertex * aSize, 0.f) + aPosition, 1.0);

    TexCoords = aTexCoords;
}