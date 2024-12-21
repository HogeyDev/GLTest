#version 410 core

in vec3 normal;
in vec2 uv_coord;

out vec4 color;

uniform sampler2D wall_texture;

void main() {
    color = texture(wall_texture, uv_coord / 2.0 + 0.5);
    // color = vec4(normal.x / 2.0 + 0.5, normal.y / 2.0 + 0.5, normal.z / 2.0 + 0.5, 1.0);
}
