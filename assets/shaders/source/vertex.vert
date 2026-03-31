#version 450

layout(location = 0) in vec2 aVertex;
layout(location = 1) in vec3 aColor;

layout(push_constant) uniform ubo {
    mat4 projection;
    mat4 model;
};

layout(location = 0) out vec3 fragColor;

void main() {
    gl_Position = projection * model * vec4(aVertex, 0.0, 1.0);

    fragColor = aColor;
}