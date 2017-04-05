#version 400 core
// Tessellation shaders with the help of:
// http://codeflow.org/entries/2010/nov/07/opengl-4-tessellation/
// http://in2gpu.com/2014/06/27/stitching-and-lod-using-tessellation-shaders-for-terrain-rendering/
// http://prideout.net/blog/?p=48

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec4 tePosition[3]; //not projected
in vec4 tePosMVP[3]; // projected with MVP
in vec3 tePatchDistance[3];
in vec4 teNormal[3];


out vec3 gFacetNormal;
out vec3 gPatchDistance;
out vec3 gTriDistance;

//For lightning
out vec4 gPointMV;
out vec4 gLightDir;
out float gDist;
out vec4 gNormal;
out vec4 pos3D;

//For texturing
out vec2 gUV;
out float gHeight;

uniform mat4 MV, MVP, light_MVP;
uniform vec3 light_pos;
uniform float grid_size;
uniform int grid_dim;
uniform sampler2D tex_noise, tex_normal;

vec2 getUV(vec3 pos){
    return (pos.xy + vec2(grid_size/2.0)) / grid_size;
}

vec2 getUV(vec2 pos){
    return (pos + vec2(grid_size/2.0)) / grid_size;
}

vec4 get3Dcoord(vec2 pos_2D){
    vec2 uv = getUV(pos_2D);
    float h = texture(tex_noise, uv).r;
    return vec4(pos_2D, h, 1.0);
}

vec4 compute_normal(vec4 current_pos) {
    float offset = 1.0/3000.0;
    vec2 posx    = current_pos.xy + vec2(offset, 0.0);
    vec2 posxneg = current_pos.xy - vec2(offset, 0.0);
    vec2 posy    = current_pos.xy + vec2(0.0, offset);
    vec2 posyneg = current_pos.xy - vec2(0.0, offset);
    vec4 dx = get3Dcoord(posx) - get3Dcoord(posxneg);
    vec4 dy = get3Dcoord(posy) - get3Dcoord(posyneg);
    return vec4(normalize(cross(dx.xyz, dy.xyz)), 1.0);
}

void main()
{
    vec4 A = tePosition[2] - tePosition[0];
    vec4 B = tePosition[1] - tePosition[0];
    gFacetNormal = normalize(cross(A.xyz, B.xyz));

    for (int i = 0; i < gl_in.length(); i++){
        //Per vertex data
        pos3D = tePosition[i];
        gPatchDistance = tePatchDistance[i];
        vec3 tmp = vec3(0);
        tmp[i] = 1;
        gTriDistance = tmp;
        gl_Position = tePosMVP[i];
        gl_ClipDistance[0] = gl_in[i].gl_ClipDistance[0];
        //For lightning
        gNormal = teNormal[i];
        vec4 pMV =  MV * tePosition[i];
        gPointMV = pMV;
        gLightDir = MV * vec4(light_pos, 1.0);
        gDist = pMV.z;
        //For texturing
        gUV = getUV(tePosition[i].xyz);
        gHeight = tePosition[i].z;
        EmitVertex();
    }
    EndPrimitive();
}



