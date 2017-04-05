#version 410 core

in vec2 vpoint;

out vec3 vPos;

uniform mat4 light_MVP, MVP;
uniform sampler2D tex_noise;
uniform float grid_size;
uniform int normal_mvp;
uniform vec3 light_pos;
uniform vec3 cam_pos;

vec2 getUV(vec2 pos){
    return (pos + vec2(grid_size/2.0)) / grid_size;
}

vec3 get3Dcoord(vec2 pos_2D){
    vec2 uv = getUV(pos_2D);
    float h = texture(tex_noise, uv).r;
    return vec3(pos_2D, h);
}

void main() {
    vPos = get3Dcoord(vpoint);
    mat4 mat = light_MVP;
    gl_Position = mat * vec4(vPos, 1.0);
}

