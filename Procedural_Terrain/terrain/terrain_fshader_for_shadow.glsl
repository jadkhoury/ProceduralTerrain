#version 330
in vec3 vPos;

layout(location = 0) out float fragmentdepth;

uniform int normal_mvp;
uniform vec3 light_pos;

void main() {
    fragmentdepth =  gl_FragCoord.z;
}
