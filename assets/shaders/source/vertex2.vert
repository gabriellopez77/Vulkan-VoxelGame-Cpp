#version 450

vec2 positions[6] = vec2[](
    vec2(-0.5f,  0.5f),  // 6
    vec2(-0.5f, -0.5f), // 5
    vec2( 0.5f, -0.5f), // 4
    vec2(-0.5f,  0.5f), // 3
    vec2( 0.5f, -0.5f), // 2
    vec2( 0.5f,  0.5f)  // 1
);

vec3 colors[] = vec3[](
    vec3(1.0, 1.0, 1.0),
    vec3(1.0, 0.0, 0.0),
    vec3(0.0, 1.0, 0.0)
);

void main() {
    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
}