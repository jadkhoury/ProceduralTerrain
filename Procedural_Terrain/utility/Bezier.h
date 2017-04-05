#pragma once

#include <algorithm>

using namespace std;

class Bezier {
    
private:
    int order;
    vector<vec3> weights;
    // Precomputed derivative vectors
    vec3 v1;// = -3.0f*weights[0] + 9.0f*weights[1] - 9.0f*weights[2] + 3.0f*weights[3];
    vec3 v2;// = 6.0f*weights[0] - 12.0f*weights[1] + 6.0f*weights[2];
    vec3 v3;// = -3.0f*weights[0] + 3.0f*weights[1];

    float getLength(vec3 vector) {
        return pow(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z, 1.0/3.0);
    }

    vec3 getDerivativeAtTime(float t) {
        return t*t*t*v1 + t*v2 + v3;
    }

public:
    Bezier(){}

    Bezier(vector<vec3> points){
        order = points.size() - 1;
        weights = points;
//        v1 = -3.0f*weights[0] + 9.0f*weights[1] - 9.0f*weights[2] + 3.0f*weights[3];
//        v2 = 6.0f*weights[0] - 12.0f*weights[1] + 6.0f*weights[2];
//        v3 = -3.0f*weights[0] + 3.0f*weights[1];
    }
    
    float getNextTime(float current_time, float step) {
        return current_time + step / getLength(current_time * current_time * v1 + current_time * v2 + v3);
    }

    vec3 getCoordinatesAtTime(float t) {
        // clamp time to max value
        t = std::min(t, 1.0f);
        t = std::max(t, 0.0f);
        
        float t2 = t * t;
        float mt = 1-t;
        float mt2 = mt * mt;
        
        if (order == 2) {
            return weights[0]*mt2 + 2.0f*weights[1]*mt*t + weights[2]*t2;
        } else if(order == 3) {
            float t3 = t2 * t;
            float mt3 = mt2 * mt;
            return weights[0]*mt3 + 3.0f*weights[1]*mt2*t + 3.0f*weights[2]*mt*t2 + weights[3]*t3;
        }
        
        return vec3(0.0f, 0.0f, 0.0f);
    }
    
    float getValueAtTime(float t) {
        // clamp time to max value
        t = std::min(t, 1.0f);
        t = std::max(t, 0.0f);
        
        float t2 = t * t;
        float mt = 1-t;
        float mt2 = mt * mt;
        
        if (order == 2) {
            return weights[0].y*mt2 + 2*weights[1].y*mt*t + weights[2].y*t2;
        } else if(order == 3) {
            float t3 = t2 * t;
            float mt3 = mt2 * mt;
            return weights[0].y*mt3 + 3*weights[1].y*mt2*t + 3*weights[2].y*mt*t2 + weights[3].y*t3;
        }
        
        return 0;
    }
    
    static Bezier generateEase() {
        vector<vec3> points = {vec3(0, 0, 0), vec3(0.33, 0, 0), vec3(0.33, 1, 0), vec3(1, 1, 0)};
        return Bezier(points);
    }
};
