/// Our heightmap is in GL_R32F format, making the use of glReadPixels impossible
/// Moreover, the texture is huge and downloading 9.46+ Mega Pixels from the GPU at each frame
/// would be an immense loss of time. So the fastest way to get the height at the camera position is
/// simply to compute it on the CPU.

#pragma once

#include "glm/gtc/type_ptr.hpp"
#include "glm/vec4.hpp"

#include <sstream>
#include <string>

#include "icg_helper.h"
#include "../noise/Noise.h"
using namespace glm;

vec4 permute(vec4 y){
    vec4 x = y;
    return mod(((x*34.0f)+1.0f)*x, 289.0f);
}

vec2 fade(vec2 t) {
    return t*t*t*(t*(t*6.0f-15.0f)+10.0f);
}

float perlinNoise(vec2 P){
    vec4 Pi = floor(vec4(P,P)) + vec4(0.0f, 0.0f, 1.0f, 1.0f);
    vec4 Pf = fract(vec4(P,P)) - vec4(0.0f, 0.0f, 1.0f, 1.0f);
    Pi = mod(Pi, 289.0f); // To avoid truncation effects in permutation
    vec4 ix = vec4(Pi.x, Pi.z, Pi.x, Pi.z);
    vec4 iy = vec4(Pi.y, Pi.y, Pi.w, Pi.w);
    vec4 fx = vec4(Pf.x, Pf.z, Pf.x, Pf.z);
    vec4 fy = vec4(Pf.y, Pf.y, Pf.w, Pf.w);
    vec4 i = permute(permute(ix) + iy);
    vec4 gx = 2.0f * fract(i / 41.0f) - 1.0f; // 1/41 = 0.024...
    vec4 gy = abs(gx) - 0.5f;
    vec4 tx = floor(gx + 0.5f);
    gx = gx - tx;
    vec2 g00 = vec2(gx.x,gy.x);
    vec2 g10 = vec2(gx.y,gy.y);
    vec2 g01 = vec2(gx.z,gy.z);
    vec2 g11 = vec2(gx.w,gy.w);
    vec4 norm = 1.79284291400159f - 0.85373472095314f *
            vec4(dot(g00, g00), dot(g01, g01), dot(g10, g10), dot(g11, g11));
    g00 *= norm.x;
    g01 *= norm.y;
    g10 *= norm.z;
    g11 *= norm.w;
    float n00 = dot(g00, vec2(fx.x, fy.x));
    float n10 = dot(g10, vec2(fx.y, fy.y));
    float n01 = dot(g01, vec2(fx.z, fy.z));
    float n11 = dot(g11, vec2(fx.w, fy.w));
    vec2 fade_xy = fade(vec2(Pf.x, Pf.y));
    vec2 n_x = mix(vec2(n00, n01), vec2(n10, n11), fade_xy.x);
    float n_xy = mix(n_x.x, n_x.y, fade_xy.y);
    return 2.3 *  n_xy;
}

float fbm(vec2 x) {
    float v = 0.0f;
    float a = 0.1;
    vec2 shift = vec2(100);
    mat2 rot = mat2(cos(0.5), sin(0.5), -sin(0.5), cos(0.50));
    for (int i = 0; i < 8; ++i) {
        v += a * perlinNoise(x);
        x =  x * 2.0f + shift;
        a *= 0.5;
    }
    return v;
}

// with the help of https://www.classes.cs.uchicago.edu/archive/2015/fall/23700-1/final-project/MusgraveTerrain00.pdf
float hyrbidMultifractal(vec2 point, float H, float lacunarity, float octaves, float offset, float gain){
    vec2 p =  point;
    float frequency, result, signal, weight;
    int i;
    bool first = true;
    float exponent_array[100];

    if(first){
        frequency = 1.0f;
        for(i=0; i<octaves; ++i){
            exponent_array[i] = pow(frequency, -H);
            frequency *= lacunarity;
        }
        first = false;
    }
    signal = offset - abs(perlinNoise(p));
    signal *= signal;
    result = signal;
    weight = 1.0f;

    for( i=1; i<octaves; i++ ) {
        p = p * lacunarity;
        weight = signal * gain;
        clamp(weight, 0.0f, 1.0f);
        signal = offset - abs(perlinNoise(p));
        signal *= signal;
        signal *= weight;
        result += signal * exponent_array[i];
    }
    return result;

}


namespace height {

// runs in less than 0.042ms
float getTerrainHeight(Noise* noise, vec2 posWorld, float render_size){
    float * params;
    params = noise->getParams();
    // unpacking the array from noise
    float density = params[0]; float tex_size = params[1]; float H = params[2];
    float lacunarity = params[3]; float octaves = params[4]; float  offset = params[5];
    float gain = params[6]; vec2 displacement = vec2(params[7], params[8]);
    vec2 posUV = (posWorld + render_size) / (2.0f * render_size);
    vec2 pos = (posUV + displacement);
    vec2 p = pos*density;
    float b2 = fbm(p*10.0f)*0.2f;
    float h1 = hyrbidMultifractal(p/8.0f, H, lacunarity, octaves, offset, gain);
    float h2 = hyrbidMultifractal(p/3.0f, H, lacunarity, octaves, offset, gain/2.0f)*2.0f;
    float h3 = hyrbidMultifractal(p*2.0f, H, lacunarity, octaves, offset, gain)*0.3f;
    return b2+h1+h2+h3-0.8f;
}

// "Gaussian" average over 5 points to smoothen the displacement
// Runs in less than 0.21ms
float getAvg(Noise* noise, vec2 posWorld, float render_size){
    float h0 = getTerrainHeight(noise, posWorld, render_size);
    float offset = 0.5;
    float hx = getTerrainHeight(noise, posWorld + vec2 (offset, 0.0), render_size);
    float hx_ = getTerrainHeight(noise, posWorld - vec2 (offset, 0.0), render_size);
    float hy = getTerrainHeight(noise, posWorld + vec2 (0.0, offset), render_size);
    float hy_ = getTerrainHeight(noise, posWorld - vec2 (0.0, offset), render_size);
    return 0.5 * h0 + 0.125 * (hx + hx_ + hy + hy_);
}

}
