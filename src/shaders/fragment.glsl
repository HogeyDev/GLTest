#version 410 core

in vec4 fizz;

out vec4 color;

void main() {
    color = vec4(fizz.x / 2.0 + 0.5, fizz.y / 2.0 + 0.5, 0.5, 1.0);
    // color = vec4(1.0f, 0.33f, 0.33f, 1.0f);
}
