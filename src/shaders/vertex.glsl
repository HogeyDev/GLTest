#version 410 core

layout (location = 0) in vec3 aPosition;

out vec4 fizz;

uniform mat4 uTransform;
uniform mat4 uProjection;
uniform mat4 uView;

void main() {
    // gl_Position = uTransform * vec4(aPosition, 1.0);
    gl_Position = uProjection * uView * uTransform * vec4(aPosition.xy, 1.0, 1.0);

    fizz = gl_Position;
}
