#version 410 core

layout (location = 0) in vec3 a_position;
layout (location = 2) in vec3 a_normal;
layout (location = 1) in vec2 a_uv_coord;

out vec2 uv_coord;
out vec3 normal;

uniform vec3 uCameraPosition;
uniform mat4 uTransform;
uniform mat4 uProjection;
uniform mat4 uView;

void main() {
    uv_coord = a_uv_coord;
    normal = a_normal;
    gl_Position = uProjection * uView * uTransform * vec4(a_position - uCameraPosition, 1.0);
}
