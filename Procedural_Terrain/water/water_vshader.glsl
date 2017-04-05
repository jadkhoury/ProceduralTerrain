#version 410 core

in vec2 vpoint;
in vec2 vtexcoord;

out vec2 uv;
out vec4 vNormal;
out vec4 viewDir;
out vec4 lightDir;
out vec4 vpointMV;
out vec4 pos3d;
out vec4 pos3dOnSphere;

uniform mat4 MVP, MV;
uniform vec3 light_pos;
uniform float grid_size;
uniform int grid_dim;

vec4 computeNormal(vec4 p){
    float offset = 0.05; //not really relevant
    vec4 px = p + vec4(offset, 0.0, 0.0, 0.0);
    vec4 py = p + vec4(0.0, offset, 0.0, 0.0);

    vec3 normal = normalize(cross(vec3(px-p), vec3(py-p)));
    return vec4(normal, 1.0);
}

vec2 getUV(vec2 pos){
    return (pos + vec2(grid_size/2.0)) / grid_size;
}

void main() {
    uv = getUV(vpoint);
    pos3d = vec4(vpoint, 0.0, 1.0);
    gl_Position = MVP * pos3d;
    //We set everything in camera space, everything in vec4
    vNormal = computeNormal(pos3d);
    vpointMV = MV * (pos3d);
    viewDir = -vpointMV; // since camera at 0,0,0
    lightDir = MV * vec4(light_pos*10.0, 1.0) - vpointMV;
}

