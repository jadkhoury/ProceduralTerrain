#version 330

in vec2 vpoint;

out vec2 vPos2D;
out vec3 vPos3D;
out vec4 vScreenPos;
out vec4 vpoint_mv;
out vec3 light_dir, view_dir;

uniform sampler2D tex_noise;
uniform float grid_size;
uniform mat4 MVP, MV, invMV;
uniform vec3 light_pos;

vec2 getUV(vec2 pos){
    return (pos + vec2(grid_size/2.0)) / grid_size;
}

vec3 get3Dcoord(vec2 pos2D){
    vec2 uv = getUV(pos2D);
    float h = texture(tex_noise, uv).r;
    return vec3(pos2D, h);
}

vec4 projectDistance(vec3 point){
    vec4 tmp = MVP * vec4(point, 1.0);
    tmp = tmp / tmp.w;
    return tmp;
}

void main() {
    vPos2D = vpoint;
    vPos3D = get3Dcoord(vpoint);
    vpoint_mv = MV * vec4(vPos3D, 1.0);
    gl_Position = MVP * vec4(vPos3D, 1.0);
    light_dir = normalize((vec4(light_pos, 1.0f) - vpoint_mv).xyz);
    view_dir = normalize(-vpoint_mv.xyz);


}

