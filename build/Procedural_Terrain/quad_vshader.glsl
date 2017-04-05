#version 330

in vec3 vpoint;
in vec2 vtexcoord;

out vec2 uv;

uniform mat4 MVP;
uniform mat4 model;

void main() {
    gl_Position = model * vec4(vpoint, 1.0);
    uv = vtexcoord;
}
