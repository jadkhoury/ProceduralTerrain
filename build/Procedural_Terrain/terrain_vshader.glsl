#version 330

in vec2 vpoint;

out vec2 vPos2D;
out vec3 vPos3D;
out vec4 vScreenPos;

uniform sampler2D tex_noise;
uniform float grid_size;
uniform int grid_dim;
uniform mat4 MVP;

vec2 getUV(vec2 pos){
    return (pos + vec2(grid_size/2.0)) / grid_size;
}

vec3 get3Dcoord(vec2 pos2D){
    vec2 uv = getUV(pos2D);
    float h = texture(tex_noise, uv).r;
    return vec3(pos2D, h);
}

vec4 projectOnScreen(vec3 point){
    vec4 tmp = MVP * vec4(point, 1.0);
    tmp = tmp / tmp.w;
    return tmp;
}

void main() {
    vPos2D = vpoint;
    vPos3D = get3Dcoord(vpoint);
    vScreenPos = projectOnScreen(vPos3D);
}
