#version 400 core
// Tessellation shaders with the help of:
// http://codeflow.org/entries/2010/nov/07/opengl-4-tessellation/
// http://in2gpu.com/2014/06/27/stitching-and-lod-using-tessellation-shaders-for-terrain-rendering/
// http://prideout.net/blog/?p=48

layout(quads, equal_spacing, cw) in;

in vec2 tcPosition[];

out vec4 tePosition;
out vec4 tePosMVP;
out vec3 tePatchDistance;
out float gl_ClipDistance[1];
out vec4 teNormal;

uniform mat4 MVP;
uniform float grid_size;
uniform int grid_dim;
uniform int clip_coef;
uniform int normals_in_fragment;
uniform sampler2D tex_noise, tex_normal;

vec2 getUV(vec2 pos){
    return (pos + vec2(grid_size/2.0)) / grid_size;
}

vec4 get3Dcoord(vec2 pos_2D){
    vec2 uv = getUV(pos_2D);
    float h = texture(tex_noise, uv).r;
    return vec4(pos_2D, h, 1.0);
}

vec4 compute_normal(vec4 current_pos) {
    float offset = 1.0/2048.0;
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
    vec2 p0 = mix(tcPosition[0], tcPosition[1], gl_TessCoord.x);
    vec2 p1 = mix(tcPosition[2], tcPosition[3], gl_TessCoord.x);
    vec2 point = mix(p0, p1, gl_TessCoord.y);

    //Per Vertex data
    tePatchDistance = gl_TessCoord;
    tePosition = get3Dcoord(point);
    tePosMVP = MVP * tePosition;
    gl_ClipDistance[0] = tePosition.z * clip_coef;
    if(normals_in_fragment > 0){
        teNormal = vec4(0.0);
    } else {
        teNormal = compute_normal(tePosition);
    }

}
