#version 330 core

in vec2 uv;

out vec3 color;

uniform sampler2D tex;

void main(){
    color = texture(tex, uv).rgb;
}
