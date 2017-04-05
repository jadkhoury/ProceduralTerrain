#pragma once

#include "icg_helper.h"
#include "glm/gtc/type_ptr.hpp"
#include "../utility/Utility.h"


using namespace glm;

class Light{
private:
    float circle_radius_ = 50.0;
    float speedFactor_ = 0.07;
    vec3 l_a = vec3(1.0f)*0.1f;
    vec3 l_d = vec3(1.0f)*0.9f;
    vec3 l_s = vec3(1.0f, 1.0f, 1.0f);
    vec3 light_pos_ = vec3(0, 0, 0);
    vec3 light_dir_ = vec3(0.0);

public:
    ///--- Pass light properties to the shader
    void uploadLightParams(GLuint program_id){
        utility::setUniformVec3(program_id, "light_pos", light_pos_);
        utility::setUniformVec3(program_id, "l_a", l_a);
        utility::setUniformVec3(program_id, "l_d", l_d);
        utility::setUniformVec3(program_id, "l_s", l_s);
    }

    vec3 getLightPos() const {
        return light_pos_;
    }

    vec3 getLightDir() const {
        return light_dir_;
    }

    void updateLightPos(float time){
        float t = time * speedFactor_;
        float x = circle_radius_ * cos(t);
        float z = circle_radius_ * sin(t);
        light_pos_ = vec3(x, 0, z);
        light_dir_ = -light_pos_;

    }
};
