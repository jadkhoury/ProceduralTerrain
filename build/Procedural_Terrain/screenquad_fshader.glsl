#version 330 core

in vec2 uv;
in vec3 vpos;

out vec3 color;

uniform sampler2D tex, tex_clouds;
uniform float tex_width;
uniform float tex_height;
uniform int grid_dim;
uniform float grid_size;
uniform vec3 cam_pos;
uniform mat4 inv_MVP, MVP;
uniform int window_h, window_w;
uniform float fovy;
uniform vec3 light_pos;

const float H = 0.8;
const float max_thickness = 0.1;
const float max_height = H + max_thickness;
const float min_height = H - max_thickness;

vec2 getUV(vec3 pos){
    return (pos.xy + vec2(grid_size/2.0)) / grid_size;
}

float findCloudThickness(vec3 originPoint, vec3 direction, float step_size){
    vec3 step = normalize(direction) * step_size;
    bool inside = false;
    vec3 p = originPoint;
    float thickness = 0.0;
    int i = 0;
    while(p.z < max_height || i < 20){
        p += step;
        //float s = abs(texture(tex_clouds, getUV(p)).r); //value of the clouds at the point uv
        float s = 0.1;
        if(p.z > (H - s) && p.z < (H + s)){
            if(!inside){
                inside = true;
            } else {
                thickness += step_size;
            }
        }
        ++i;
    }
    return thickness;
}

vec3 convertToSRGB(vec3 linear){
    float r = pow(linear.x, 1.0/2.2);
    float g = pow(linear.y, 1.0/2.2);
    float b = pow(linear.z, 1.0/2.2);
    return vec3(r,g,b);

}

void main() {

    color = texture(tex, uv).rgb;


}
