#pragma once

#include "icg_helper.h"
#include "glm/gtc/type_ptr.hpp"
#include "../utility/Utility.h"
#define MIRRORED 1
#define STANDARD 0

using namespace glm;

class Transformations {

private:
    int window_w_;
    int window_h_;
    vec3 cam_pos_;
    mat4 projection_;
    float fovy_;

    mat4 view_;
    mat4 MVP_;
    mat4 MV_;
    mat4 inv_MV_;
    mat4 inv_MVP_;

    mat4 view_mirrored_;
    mat4 MVP_mirrored_;
    mat4 MV_mirrored_;
    mat4 inv_MV_mirrored_;
    mat4 inv_MVP_mirrored_;

    mat4 light_MVP_;
    vec3 right_vector_;

    vec3 light_pos_;

    float time;

    bool first_ = true;

    int tessellation_demo_ = 0;
    float tessellation_factor_ = 1.0;
    int normals_in_fragment = 1;
    int raymarching = 0;

    vec2 noise_displacement_;


public:
    void updateTransformations(const mat4& lookAt, const mat4& lookAt_mirrored, vec3 light_position, vec2 disp){
        view_ = lookAt;
        view_mirrored_ = lookAt_mirrored;
        MVP_ = projection_ * view_;
        MV_ = view_;
        inv_MV_ = transpose(inverse(MV_));
        inv_MVP_ = inverse(MVP_);
        MVP_mirrored_ = projection_*view_mirrored_;
        MV_mirrored_ = view_mirrored_;
        inv_MV_mirrored_ = transpose(inverse(MV_mirrored_));
        inv_MVP_mirrored_ = inverse(MVP_mirrored_);
        light_MVP_ = computeShadowMVP(light_position);
        noise_displacement_ = disp;
        light_pos_ = light_position;
        time = glfwGetTime();

    }

    void updateProjection(float fovy, float near, float width, float height){
        window_w_ = width;
        window_h_ = height;
        fovy_ = fovy;
        projection_ =  perspective(fovy_, (float)window_w_/(float)window_h_, near, 100.0f);
    }



    //Upload the transformations matrices to the GPU along with some useful data
    void uploadToGPU(GLuint pid, int is_mirrored){
        if(is_mirrored == MIRRORED){
            utility::setUniformMat4(pid, "MVP", MVP_mirrored_);
            utility::setUniformMat4(pid, "MV", MV_mirrored_);
            utility::setUniformMat4(pid, "inv_MV", inv_MV_mirrored_);
        } else {
            utility::setUniformMat4(pid, "MVP", MVP_);
            utility::setUniformMat4(pid, "MV", MV_);
            utility::setUniformMat4(pid, "inv_MV", inv_MV_);
        }
        utility::setUniformVec3(pid, "cam_pos", cam_pos_);
        utility::setUniformInt(pid, "window_w", window_w_);
        utility::setUniformInt(pid, "window_h", window_h_);
        utility::setUniformFloat(pid, "time", time);
        utility::setUniformFloat(pid, "PI", 3.14159265359);
        utility::setUniformMat4(pid, "light_MVP", light_MVP_);
        utility::setUniformVec2(pid, "noise_displacement", noise_displacement_);
        utility::setUniformInt(pid, "tessellation_demo", tessellation_demo_);
        utility::setUniformInt(pid, "normals_in_fragment", normals_in_fragment);
        utility::setUniformFloat(pid, "tessellation_factor", tessellation_factor_);
        utility::setUniformInt(pid, "raymarching", raymarching);
    }

    void uploadScreenQuadParam(GLuint pid, int is_mirrored){
        if(is_mirrored == MIRRORED){
            utility::setUniformMat4(pid, "inv_MVP", inv_MVP_mirrored_);
            utility::setUniformMat4(pid, "MVP", MVP_mirrored_);

        } else {
            utility::setUniformMat4(pid, "inv_MVP", inv_MVP_);
            utility::setUniformMat4(pid, "MVP", MVP_);

        }
        utility::setUniformVec3(pid, "cam_pos", cam_pos_);
        utility::setUniformFloat(pid, "time", time);
        utility::setUniformInt(pid, "window_w", window_w_);
        utility::setUniformInt(pid, "window_h", window_h_);
        utility::setUniformFloat(pid, "fovy", fovy_);
    }

    mat4 computeShadowMVP(vec3 light_position){
        vec3 dir = normalize(-light_position);
        right_vector_ = vec3(0.0, 1.0, 0.0); // light rotating around y axis
        vec3 up = -normalize(cross(dir, right_vector_));
        if(up.z < 0) up = -up;
        vec3 look = vec3(0.0);
        mat4 V = glm::lookAt(light_position, look, vec3(0.0, 0.0, 1.0));
        mat4 P = glm::ortho(-30.0f, 30.0f, -30.0f, 30.0f, 20.0f, 120.0f);
        return P*V;
    }

    void updateCamPos(const vec3& new_pos){
        cam_pos_ = new_pos;
    }

    mat4 MVP() const{
        return MVP_;
    }
    mat4 MV() const{
        return MV_;
    }
    mat4 invMV() const{
        return inv_MV_;
    }
    mat4 MVP_mirrored() const{
        return MVP_mirrored_;
    }
    mat4 MV_mirrored() const{
        return MV_mirrored_;
    }
    mat4 invMV_mirrored() const{
        return inv_MV_mirrored_;
    }
    vec3 camPos() const{
        return cam_pos_;
    }

    mat4 getProjection() const{
        return projection_;
    }
    mat4 getView() const{
        return view_;
    }

    void tweak_tess_factor(int incr){
        float factor = 0.05;
        float t = tessellation_factor_ + incr*factor;
        if(t>0.01) tessellation_factor_ = t;
    }
    void toggle_tess_demo(){
        tessellation_demo_ = 1 - tessellation_demo_;
        cout << "demo: " << tessellation_demo_ << endl;
    }
    void toggle_normal_mode(){
        normals_in_fragment = 1 - normals_in_fragment;
        string t = (normals_in_fragment > 0) ? "fragment shader" : "Tess. Eval. shader";
        cout << "normals computed in: " << t << endl;
    }
    void toggle_raymarching(){
        raymarching = 1 - raymarching;
        string t = (raymarching > 0) ? "ON" : "OFF";
        cout << "Ray Marching " << t << endl;
    }

};



