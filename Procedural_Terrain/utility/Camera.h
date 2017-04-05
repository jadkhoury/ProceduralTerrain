#pragma once

#include "icg_helper.h"
#include "glm/gtc/type_ptr.hpp"

using namespace glm;

class Camera {

private:
    //Initialisation Values
    vec3 INIT_POS = vec3(0, 0, 1.00);
    vec3 INIT_LOOK = vec3(1.46f, 0.026f, 0.77f);

    vec3 INIT_UP = vec3(0.0f, 0.0f, 1.0f);
    
    //Basic vectors
    vec3 pos_;
    vec3 look_;
    vec3 up_;
    vec3 right_;
    
    //Controls
    float DELTA_H_ANGLE = M_PI;
    float DELTA_V_ANGLE = M_PI;
    vec3 direction_, new_look_;
    mat4 hor_rotation_, vert_rotation_;
    int i = 0;

public:
    Camera(){
        pos_ = INIT_POS;
        look_ = INIT_LOOK;
        up_ = INIT_UP;
        right_ = normalize(cross(look_, up_));
    }

    vec3 getPosition(){
        return pos_;
    }
    
    float getHeight(){
        return pos_.z;
    }
    
    vec3 getLook(){
        return look_;
    }

    void setLook(vec3 newLook){
        look_ = newLook;
    }

    vec3 getUp(){
        return up_;
    }
    vec2 getDirection(){
        return vec2(look_ - pos_);
    }

    mat4 lookAt() {
        return glm::lookAt(pos_, look_, up_);
    }

    mat4 lookAtMirrored() {
        return glm::lookAt(vec3(pos_.x, pos_.y, -pos_.z), vec3(look_.x, look_.y, -look_.z), up_);
    }

    void rotateMouse(float horiz_shift, float vert_shift){
        mat4 horiz_rotation = glm::rotate(IDENTITY_MATRIX, horiz_shift, up_);
        mat4 vert_rotation = glm::rotate(IDENTITY_MATRIX, vert_shift, right_);
        vec3 direction = look_ - pos_;

        look_ = mat3(horiz_rotation) * mat3(vert_rotation) * direction + pos_;
        right_ = mat3(horiz_rotation) * mat3(vert_rotation) * right_;
    }

    void moveVertically(float increment){
        pos_ = vec3(pos_.x, pos_.y, pos_.z + increment);
        look_ = vec3(look_.x, look_.y, look_.z + increment);
    }
    void setHeight(float h){
        moveVertically(h-pos_.z);
    }
};
