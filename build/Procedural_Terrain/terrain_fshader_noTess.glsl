#version 330
in vec3 vPos;
in vec2 vPos2D;
in vec3 vPos3D;
in vec4 vScreenPos;
in vec4 vpoint_mv;
in vec3 light_dir, view_dir;

out vec3 color;

void main() {
    vec3 vpoint_mv_3 = vpoint_mv.xyz;
    vec3 t_x = normalize(dFdx(vpoint_mv_3));
    vec3 t_y = normalize(dFdy(vpoint_mv_3));
    vec3 triangle_normal = cross(t_x,t_y);
    vec3 reflect_dir_mv = normalize(reflect(-light_dir, triangle_normal));
    float diffuse_angle = max(dot(triangle_normal,light_dir),0);
    vec3 diffuse = vec3(1.0)*diffuse_angle;

    color = diffuse;

}
