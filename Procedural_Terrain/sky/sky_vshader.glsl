#version 330 core

in vec3 vpoint;
in vec2 vtexcoord;

out vec2 uv;

uniform mat4 MVP;

mat4 S (float sx, float sy, float sz){
    mat4 S = mat4(1.0);
    S[0][0] = sx;
    S[1][1] = sy;
    S[2][2] = sz;
    return S;
}

mat4 R(float degrees){
    mat4 R = mat4(1);
    float alpha = radians(degrees);
    R[0][0] =  cos(alpha);
    R[0][1] =  sin(alpha);
    R[1][0] = -sin(alpha);
    R[1][1] =  cos(alpha);
    return R;
}

mat4 T(float tx, float ty, float tz){
    mat4 T = mat4(1);
    T[3][0] = tx;
    T[3][1] = ty;
    T[3][2] = tz;
    return T;
}

void main(){
    gl_Position = MVP * vec4(vpoint,1);
    uv = vtexcoord;
}
