#pragma once

#include "Bezier.h"
#include "Controls.h"

using namespace std;

class CameraBezierPath {
    
private:
    Camera cam_;
    Controls controls_;
    
    long number_frames_since_start_ = 0;
    Bezier current_curve_;
    Bezier current_look_;
    
    float next_time = 0;
    float MOVE_STEP = 0.009;
    float timer = 0.0;
    float wait_time = 5.0;

    
    std::vector<vec3>::iterator iterator_;
    std::vector<vec3>::iterator iterator_look_;

    vector<vec3> path_points_ = {
        vec3(0.0f, 0.0f, 1.0f), vec3(-0.0260416, -0.1341146, 0.230385), vec3(0.10, -0.05, -0.0),
        vec3(0.194987, 0.0442708, 0.230385), vec3(0.194987, 0.0442708, 2.0), vec3(0.3076f, 0.0736f, 0.0),
        vec3(0.3076f, 0.0736f, 0.2901f), vec3(0.3076f, 0.0736f, 0.2901f), vec3(0.3076f, 0.0736f, 4.0f),
        vec3(0.3076f, 0.0736f, 4.0f), vec3(0.3076f, 0.0736f, 4.0f), vec3(1.413, -0.9427, 2.6252),
        vec3(1.413, -0.9427, 2.6252), vec3(1.413, -0.9427, 2.6252), vec3(2.5201f, -1.959f, 1.2504f),
        vec3(2.5201f, -1.959f, 1.2504f)
    };

    vector<vec3> look_values_ = {
        vec3(1.46f, 0.026f, 0.77f), vec3(1.46f, 0.026f, 0.77f), vec3(1.29012, 0.695505, 0.0475765),
        vec3(1.29012, 0.695505, 0.0475765), vec3(1.21f, 0.82f, 0.13f), vec3(0.3182f, 1.4421f, 0.2248f),
        vec3(0.3182f, 1.4421f, 0.2248f),vec3(2.0f, 1.0f, 0.2248f), vec3(0.94f, -1.005f, 3.6619f),
        vec3(0.94f, -1.005f, 3.6619f), vec3(2.5201f, -1.959f, 2.0f), vec3(2.5201f, -1.959f, 2.0),
        vec3(2.5201f, -1.959f, 2.0), vec3(2.5201f, -1.959f, 2.0), vec3(0.071954, 1.97228, 1.37167),
        vec3(0.071954, 1.97228, 1.37167)
    };
    
    void getNextCurve() {
        std::vector<vec3>::iterator last_in_range = next(iterator_, 3);
        vector<vec3> current_curve_points(iterator_, next(last_in_range, 1));//(excludes last_in_range)

        current_curve_ = Bezier(current_curve_points);
        
        number_frames_since_start_ = 0;
        next_time = 0;
        timer = 0.0;
        
        iterator_ = last_in_range; //We keep the last point as the new first one
    }
    void getNextLook() {

        std::vector<vec3>::iterator last_in_range_look = next(iterator_look_, 3);
        vector<vec3> current_look_points(iterator_look_, next(last_in_range_look, 1));   //(excludes last_in_range)

        current_look_ = Bezier(current_look_points);

        iterator_look_ = last_in_range_look; //We keep the last point as the new first one
    }


    float distance(vec3 a, vec3 b){
        vec3 tmp = (a-b);
        return abs(sqrt(tmp.x * tmp.x + tmp.y * tmp.y + tmp.z * tmp.z));
    }
    
public:
    CameraBezierPath(){}
    
    CameraBezierPath(Camera cam, Controls controls){
        cam_ = cam;
        controls_ = controls;
        iterator_ = path_points_.begin();
        iterator_look_ = look_values_.begin();
        getNextCurve();
        getNextLook();
    }
    
    // Return false if we are at the end of the path
    bool move(double delta_time) {
        float advancement_per_sec = 0.1;
        float step_per_frame = delta_time * advancement_per_sec;
        vec3 current_position = controls_.getRealCamPos();
        if(distance(current_position, path_points_.back()) < 0.01) {
            return false;
        } else if(distance(current_position, *iterator_) < 0.01) {
            timer += delta_time;
            if(timer > wait_time) {
                getNextCurve();
                getNextLook();
            }
        }
        
       // next_time = current_curve_.getNextTime(next_time, MOVE_STEP);
        vec3 next_position = current_curve_.getCoordinatesAtTime(next_time);

        vec3 aim = current_look_.getCoordinatesAtTime(next_time);
        
        float frontal_shift = next_position.x - current_position.x;
        float lateral_shift = next_position.y - current_position.y;
        float vertical_shift = current_position.z - next_position.z;
        
        controls_.move(frontal_shift, lateral_shift, vertical_shift);
        controls_.aimTo(aim);
        
        number_frames_since_start_++;
        next_time = next_time + MOVE_STEP;
        
        return true;
    }

    void tweak_move_step(float incr){
        MOVE_STEP += incr;
    }
};
