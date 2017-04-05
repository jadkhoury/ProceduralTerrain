#version 400 core
layout(vertices = 4) out;
// Tessellation shaders with the help of:
// http://codeflow.org/entries/2010/nov/07/opengl-4-tessellation/
// http://in2gpu.com/2014/06/27/stitching-and-lod-using-tessellation-shaders-for-terrain-rendering/
// http://prideout.net/blog/?p=48

#define ID gl_InvocationID

in vec2 vPos2D[];
in vec3 vPos3D[];
in vec4 vScreenPos[];

out vec2 tcPosition[];

uniform int window_w, window_h;
uniform int clip_coef;
uniform mat4 MV;
uniform float tessellation_factor;

bool offscreen(vec4 vertex){
    float d = 1.2;
    bool hor = (vertex.x < -d || vertex.x > d);
    bool vert = (vertex.y < -d || vertex.y > d);
    return (hor || vert);
}


float level(vec4 p0, vec4 p1, vec3 p0_3D, vec3 p1_3D){
    if(clip_coef > 0 || (offscreen(p0) && offscreen(p1))) return 1.0; // We don't nered great precision for the reflection
    vec2 screen_size = vec2(window_w, window_h);
    float downFactor = 3.0;
    p0.xy = p0.xy*screen_size*0.5;
    p1.xy = p1.xy*screen_size*0.5;
    float f = abs(distance(p0, p1));
    if(p0_3D.z >0.9 && p1_3D.z >0.9) return 5.0; // We need more precision at the peaks
    if(p0_3D.z < -0.05 && p1_3D.z < -0.05) downFactor *= 2.0;
    return f/downFactor * tessellation_factor;

}


void main() {
    tcPosition[ID] = vPos2D[ID];
    if (ID == 0) {
        vec4 p0 = vScreenPos[0]; vec3 p0_3D = vPos3D[0];
        vec4 p1 = vScreenPos[1]; vec3 p1_3D = vPos3D[1];
        vec4 p2 = vScreenPos[2]; vec3 p2_3D = vPos3D[2];
        vec4 p3 = vScreenPos[3]; vec3 p3_3D = vPos3D[3];
        if(offscreen(p0) && offscreen(p1) && offscreen(p2) && offscreen(p3)){
            gl_TessLevelInner[0] = 0;
            gl_TessLevelInner[1] = 0;
            gl_TessLevelOuter[0] = 0;
            gl_TessLevelOuter[1] = 0;
            gl_TessLevelOuter[2] = 0;
            gl_TessLevelOuter[3] = 0;
        } else {
            float lvl0 = level(p0, p2, p0_3D, p2_3D);
            float lvl1 = level(p0, p1, p0_3D, p1_3D);
            float lvl2 = level(p1, p3, p1_3D, p3_3D);
            float lvl3 = level(p2, p3, p2_3D, p3_3D);

            gl_TessLevelInner[0] = (lvl0+lvl2)/2.0;
            gl_TessLevelInner[1] = (lvl1+lvl3)/2.0;
            gl_TessLevelOuter[0] = lvl0;
            gl_TessLevelOuter[1] = lvl1;
            gl_TessLevelOuter[2] = lvl2;
            gl_TessLevelOuter[3] = lvl3;
        }
    }
}
