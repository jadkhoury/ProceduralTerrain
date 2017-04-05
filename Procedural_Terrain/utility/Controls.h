#pragma once

#include <algorithm>

#include <GLFW/glfw3.h>

#include "../utility/Camera.h"
#include "../noise/Noise.h"
#include "../utility/Bezier.h"
#include "../utility/Height.h"

using namespace std;

class Controls {
    
private:
    
    Camera* cam_;
    Noise* noise_;
    
    float init_shift_pace_ = 100.0f;
    float offset_shift_pace_ = 40.0f;
    float horiz_shift_pace_ = init_shift_pace_;
    float vert_shift_pace_ = init_shift_pace_;
    
    Bezier ease = Bezier::generateEase();
    
    bool move_up_previous_state_ = false;
    bool move_down_previous_state_ = false;
    bool move_left_previous_state_ = false;
    bool move_right_previous_state_ = false;
    
    int elapsed_time_since_up_pressed = -1;
    int elapsed_time_since_down_pressed = -1;
    int elapsed_time_since_left_pressed = -1;
    int elapsed_time_since_right_pressed = -1;
    
    float DELTA_H_ANGLE = M_PI;
    float DELTA_V_ANGLE = M_PI;
    int fps_mode = 0;

    //Tests for physical movements
    float mass = 100.0;
    float k = 0.5;
    float push = 5000.0;
    float v_up = 0.0;
    float v_down = 0.0;
    float v_right = 0.0;
    float v_left = 0.0;
    float a_up, a_down, a_right, a_left;
    bool first = true;
    float last_t, t, dt;

    void udpateHorizontalShiftPace(float time) {
        time *= 10;
        horiz_shift_pace_ = ease.getValueAtTime((init_shift_pace_ - time)/init_shift_pace_)*init_shift_pace_; //We rescale between 0 and 1 to query easeIn()
    }
    
    void updateVerticalShiftPace(float time) {
        time *= 10;
        vert_shift_pace_ = ease.getValueAtTime((init_shift_pace_ - time)/init_shift_pace_)*init_shift_pace_; //We rescale between 0 and 1 to query easeIn()
    }
    
public:
    
    Controls() {}
    
    Controls(Camera* cam, Noise* noise) {
        cam_ = cam;
        noise_ = noise;
    }
    
    void move(bool move_up, bool move_down, bool move_left, bool move_right) {
        if (move_up && move_down) {
            move_up = move_up_previous_state_;
            move_down = move_down_previous_state_;
        }
        if (move_left && move_right) {
            move_left = move_left_previous_state_;
            move_right = move_right_previous_state_;
        }
        
        if(vert_shift_pace_ == init_shift_pace_ && !move_up) {
            elapsed_time_since_up_pressed = -1;
        } else if(vert_shift_pace_ == init_shift_pace_ && !move_down) {
            elapsed_time_since_down_pressed = -1;
        }
        
        if(horiz_shift_pace_ == init_shift_pace_ && !move_left) {
            elapsed_time_since_left_pressed = -1;
        } else if(horiz_shift_pace_ == init_shift_pace_ && !move_right) {
            elapsed_time_since_right_pressed = -1;
        }
        
        vec3 up = normalize(cam_->getUp());
        vec3 dir = vec3(normalize(cam_->getDirection()), 0.0);
        vec3 side = normalize(cross(up, dir));
        
        vec2 vert_shift = vec2(0.0);
        
        if (move_up && move_up_previous_state_ && elapsed_time_since_down_pressed == -1) {
            vert_shift = vec2(dir);
            if (vert_shift_pace_ > 0) {
                elapsed_time_since_up_pressed++;
            }
            updateVerticalShiftPace(elapsed_time_since_up_pressed);
        } else if (move_up && !move_down && !move_up_previous_state_ && elapsed_time_since_down_pressed == -1) {
            vert_shift = vec2(dir);
            elapsed_time_since_up_pressed = 1;
            updateVerticalShiftPace(elapsed_time_since_up_pressed);
        }
        
        if (move_down && move_down_previous_state_ && elapsed_time_since_up_pressed == -1) {
            vert_shift = -vec2(dir);
            if (vert_shift_pace_ > 0) {
                elapsed_time_since_down_pressed++;
            }
            updateVerticalShiftPace(elapsed_time_since_down_pressed);
        } else if (move_down && !move_up && !move_down_previous_state_ && elapsed_time_since_up_pressed == -1) {
            vert_shift = -vec2(dir);
            elapsed_time_since_down_pressed = 1;
            updateVerticalShiftPace(elapsed_time_since_down_pressed);
        }
        
        if(vert_shift_pace_ < init_shift_pace_ && !move_up && elapsed_time_since_up_pressed > 0) {
            elapsed_time_since_up_pressed--;
            updateVerticalShiftPace(elapsed_time_since_up_pressed);
            vert_shift = vec2(dir);
            
            if(elapsed_time_since_up_pressed == 0) {
                elapsed_time_since_down_pressed = 0;
            }
        } else if(vert_shift_pace_ < init_shift_pace_ && !move_down && elapsed_time_since_down_pressed > 0) {
            elapsed_time_since_down_pressed--;
            updateVerticalShiftPace(elapsed_time_since_down_pressed);
            vert_shift = -vec2(dir);
            
            if(elapsed_time_since_down_pressed == 0) {
                elapsed_time_since_up_pressed = 0;
            }
        }
        
        vec2 horiz_shift = vec2(0.0);
        
        if (move_left && move_left_previous_state_ && elapsed_time_since_right_pressed == -1) {
            horiz_shift = vec2(side);
            if (horiz_shift_pace_ > 0) {
                elapsed_time_since_left_pressed++;
            }
            udpateHorizontalShiftPace(elapsed_time_since_left_pressed);
        } else if (move_left && !move_right && !move_left_previous_state_ && elapsed_time_since_right_pressed == -1) {
            horiz_shift = vec2(side);
            elapsed_time_since_left_pressed = 1;
            udpateHorizontalShiftPace(elapsed_time_since_left_pressed);
        }
        
        if (move_right && move_right_previous_state_ && elapsed_time_since_left_pressed == -1) {
            horiz_shift = -vec2(side);
            if (horiz_shift_pace_ > 0) {
                elapsed_time_since_right_pressed++;
            }
            udpateHorizontalShiftPace(elapsed_time_since_right_pressed);
        } else if (move_right && !move_left && !move_right_previous_state_ && elapsed_time_since_left_pressed == -1) {
            horiz_shift = -vec2(side);
            elapsed_time_since_right_pressed = 1;
            udpateHorizontalShiftPace(elapsed_time_since_right_pressed);
        }
        
        if(horiz_shift_pace_ < init_shift_pace_ && !move_left && elapsed_time_since_left_pressed > 0) {
            elapsed_time_since_left_pressed--;
            udpateHorizontalShiftPace(elapsed_time_since_left_pressed);
            horiz_shift = vec2(side);
            
            if(elapsed_time_since_left_pressed == 0) {
                elapsed_time_since_right_pressed = 0;
            }
        } else if(horiz_shift_pace_ < init_shift_pace_ && !move_right && elapsed_time_since_right_pressed > 0) {
            elapsed_time_since_right_pressed--;
            udpateHorizontalShiftPace(elapsed_time_since_right_pressed);
            horiz_shift = -vec2(side);
            
            if(elapsed_time_since_right_pressed == 0) {
                elapsed_time_since_left_pressed = 0;
            }
        }

        if (vert_shift.x != 0.0 || vert_shift.y != 0.0 || horiz_shift.x != 0.0 || horiz_shift.y != 0.0) {
            noise_->move(vert_shift/(vert_shift_pace_+offset_shift_pace_) + horiz_shift/(horiz_shift_pace_+offset_shift_pace_));

        }

        if(fps_mode > 0){
            float h = height::getAvg(noise_, vec2(0.0), 25.0);
            h = (h < 0.0) ? 0.0 : h;
            float h_offset = 0.5;
            cam_->setHeight(h+h_offset);
        }
        move_up_previous_state_ = move_up;
        move_down_previous_state_ = move_down;
        move_left_previous_state_ = move_left;
        move_right_previous_state_ = move_right;
    }
    
    void move(float frontal_shift, float lateral_shift, float vertical_shift) {
        noise_->move(vec2(frontal_shift, lateral_shift));
        moveVertically(vertical_shift, 0);
    }
    
    void moveAround(double x_shift, double y_shift){
        cam_->rotateMouse(x_shift * DELTA_H_ANGLE, y_shift * DELTA_V_ANGLE);
    }
    
    void moveVertically(float increment, float minHeightTerrain){
        float scaledIncrement = - increment/10;
        if(fps_mode == 0)
            cam_->moveVertically(scaledIncrement);
    }

    void toggle_fps(){
        fps_mode = 1 - fps_mode;
    }
    void fps_off(){
        fps_mode = 0;
    }

    void aimTo(vec3 aim) {
        cam_->setLook(aim);
    }
    
    vec3 getRealCamPos() {
        return vec3(getTerrainDisplacement(), cam_->getPosition().z);
    }
    
    vec2 getTerrainDisplacement() {
        return noise_->getDisplacement();
    }

    void tweak_max_displacement_speed(float incr){
        offset_shift_pace_ -= incr;
    }

    void physicalMove(bool move_up, bool move_down, bool move_left, bool move_right){
        t = glfwGetTime();
        if(first){
            dt = 1.0f/60.0f;
            first = false;
        } else {
            dt = t - last_t;
        }
        last_t = t;
        vec3 up = normalize(cam_->getUp());
        vec3 dir = vec3(normalize(cam_->getDirection()), 0.0);
        vec3 side = normalize(cross(up, dir));



        //acceleration
        a_up    = (move_up    * push - k * v_up * v_up)       / mass;
        a_down  = (move_down  * push - k * v_down * v_down)   / mass;
        a_right = (move_right * push - k * v_right * v_right) / mass;
        a_left  = (move_left  * push - k * v_left * v_left)   / mass;

        //Velocity
        v_up    += a_up    * dt;
        v_down  += a_down  * dt;
        v_right += a_right * dt;
        v_left  += a_left  * dt;


        //Displacement
        vec2 d = vec2(dir) * v_up * dt;
        d += vec2(-dir) * v_down * dt;
        d += vec2(side) * v_left * dt;
        d += vec2(-side) * v_right * dt;

        noise_->move(d*0.001f);

    }
};
