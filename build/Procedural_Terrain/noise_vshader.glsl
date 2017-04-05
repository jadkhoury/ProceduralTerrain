#version 330 core

in vec3 vpoint;
in vec2 vtexcoord;

out vec2 uv;

uniform mat4 MVP;

void main() {
    gl_Position = vec4(vpoint, 1.0);
    uv = vtexcoord;
}
