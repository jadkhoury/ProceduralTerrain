#pragma once

#include "glm/gtc/type_ptr.hpp"

#include <sstream>
#include <string>

#include "icg_helper.h"


using namespace glm;

class Physics{
private:
    float push_force_ = 1000; //N
    float mass_ = 100;//kg
    float k = 0.2; // friction coef
    vec4 velocity_ = vec4(0.0);
    bvec4 push_ = false;


public :
    void update(float deltaT, bool up, bool down, bool left, bool right){
        push_ = vec4(up, down, left, right);
        for(int i = 0; i<4; ++i){
            velocity_(i) += ((push_(i)*push_force_-k*velocity_(i)*velocity_(i))/mass_)*deltaT;
        }
        float forward = velocity_(0) - velocity_(1);
        float right = velocity_(2) - velocity(3);
    }

}
