#pragma once

#include "glm/gtc/type_ptr.hpp"

#include "icg_helper.h"

#include "../utility/Utility.h"

#define CLOUDS_MAP 1
#define HEIGHT_MAP 0

using namespace glm;

class Noise {
    
private:
    GLuint vertex_array_id_;
    GLuint vertex_buffer_object_;
    GLuint program_id_;
    GLuint texture_noise_id_;
    GLuint texture_noise_id_2_;
    bool modified_ = false;
    bool displaced_ = false;
    int tex_size_;
    //Parameters for the Heightmap
    float H_ = 0.7;
    float lacunarity_ = 4.0;
    float octaves_ = 5.0;
    float offset_ = 0.9;
    float gain_ = 1.0;
    float density_ = 7.5;
    vec2 displacement_;
    vec2 new_displacement_;
    //This class will render both Heightmap and clouds map
    int kind_;


    float roundToPrecision(float precision, float value){
        return precision * round(value/precision);
    }
    vec2 roundToPrecision(float precision, vec2 value){
        float x = roundToPrecision(precision, value.x);
        float y = roundToPrecision(precision, value.y);
        return vec2(x,y);

    }
    void uploadHeightParams(){
        utility::setUniformFloat(program_id_, "H", H_);
        utility::setUniformFloat(program_id_, "lacunarity", lacunarity_);
        utility::setUniformFloat(program_id_, "octaves", octaves_);
        utility::setUniformFloat(program_id_, "offset", offset_);
        utility::setUniformFloat(program_id_, "gain", gain_);
        utility::setUniformFloat(program_id_, "density", density_);
        utility::setUniformInt(program_id_, "modified", modified_);
        utility::setUniformInt(program_id_, "displaced", displaced_);
        utility::setUniformVec2(program_id_, "displacement", displacement_);
        utility::setUniformVec2(program_id_, "new_displacement", new_displacement_);
    }

    void printParam(void){
        cout << "H = " << H_ << endl;
        cout << "lacunarity_ = " << lacunarity_ << endl;
        cout << "octaves_ = " << octaves_ << endl;
        cout << "offset_ = " << offset_ << endl;
        cout << "gain_ = " << gain_ << endl << endl;
    }

public:
    void init(int kind, int tex_size, GLuint tex_noise = -1, GLuint tex_noise_2 = -1) {
        tex_size_ = tex_size;
        displacement_ = vec2(0.0);
        cout << "tex size in init: " << tex_size_ << endl;

        // compile the shaders
        program_id_ = icg_helper::LoadShaders("noise_vshader.glsl",
                                              "noise_fshader.glsl");
        if(!program_id_) {
            exit(EXIT_FAILURE);
        }

        glUseProgram(program_id_);
        // vertex one vertex Array
        glGenVertexArrays(1, &vertex_array_id_);
        glBindVertexArray(vertex_array_id_);

        // vertex coordinates
        {
            const GLfloat vertex_point[] = { /*V1*/ -1.0f, -1.0f, 0.0f,
                                             /*V2*/ +1.0f, -1.0f, 0.0f,
                                             /*V3*/ -1.0f, +1.0f, 0.0f,
                                             /*V4*/ +1.0f, +1.0f, 0.0f};
            // buffer
            glGenBuffers(1, &vertex_buffer_object_);
            glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_point), vertex_point, GL_STATIC_DRAW);

            // attribute
            GLuint vertex_point_id = glGetAttribLocation(program_id_, "vpoint");
            glEnableVertexAttribArray(vertex_point_id);
            glVertexAttribPointer(vertex_point_id, 3, GL_FLOAT, DONT_NORMALIZE,  ZERO_STRIDE, ZERO_BUFFER_OFFSET);
        }

        // texture coordinates
        {
            const GLfloat vertex_texture_coordinates[] = { /*V1*/ 0.0f, 0.0f,
                                                           /*V2*/ 1.0f, 0.0f,
                                                           /*V3*/ 0.0f, 1.0f,
                                                           /*V4*/ 1.0f, 1.0f};

            // buffer
            glGenBuffers(1, &vertex_buffer_object_);
            glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_texture_coordinates), vertex_texture_coordinates, GL_STATIC_DRAW);

            // attribute
            GLuint vertex_texture_coord_id = glGetAttribLocation(program_id_,  "vtexcoord");
            glEnableVertexAttribArray(vertex_texture_coord_id);
            glVertexAttribPointer(vertex_texture_coord_id, 2, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);
        }

        if(kind_ == HEIGHT_MAP){
            texture_noise_id_ = tex_noise;
            utility::setUniformInt(program_id_, "tex_noise", 1);

            texture_noise_id_2_ = tex_noise_2;
            utility::setUniformInt(program_id_, "tex_noise_2", 2);
        }

        utility::setUniformInt(program_id_, "tex_size", tex_size);
        utility::setUniformInt(program_id_, "kind", kind_);


        // to avoid the current object being polluted
        glBindVertexArray(0);
        glUseProgram(0);

        cout << "--Noise initialized" << endl;
    }

    void cleanup() {
        glBindVertexArray(0);
        glUseProgram(0);
        glDeleteBuffers(1, &vertex_buffer_object_);
        glDeleteProgram(program_id_);
        glDeleteVertexArrays(1, &vertex_array_id_);
        glDeleteTextures(1, &texture_noise_id_);
    }

    void drawHeightMap(int pass = -1, int last_buffer = -1) {
        glUseProgram(program_id_);
        glBindVertexArray(vertex_array_id_);

        utility::setUniformInt(program_id_, "pass", pass);
        utility::setUniformInt(program_id_, "last_buffer", last_buffer);
        uploadHeightParams();

        if (last_buffer == 1){
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, texture_noise_id_);
        } else {
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, texture_noise_id_2_);
        }

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        modified_ = false;
        displaced_ = false;
        new_displacement_ = vec2(0.0);

        glBindVertexArray(0);
        glUseProgram(0);
    }

    void drawCloudsMap(){
        glUseProgram(program_id_);
        glBindVertexArray(vertex_array_id_);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        displaced_ = false;
        cout << "tex size : " << tex_size_ << endl;

        glBindVertexArray(0);
        glUseProgram(0);
    }

    void move(vec2 delta){
        //We set the displacement to a multiple of the "texel size" such that the
        //interpolation doesn't smooth the terrainw
        vec2 d = roundToPrecision(1.0/float(tex_size_), delta);
        new_displacement_ = d;
        displacement_ = displacement_ + d;
        displaced_ = true;
    }

    void tweakH(float incr){
        H_ += incr;
        modified_ = true;
        printParam();
    }

    void tweakLacunarity(float incr){
        lacunarity_ += incr;
        modified_ = true;
        printParam();
    }
    void tweakOctaves(float incr){
        octaves_ += incr;
        modified_ = true;
        printParam();
    }
    void tweakOffset(float incr){
        offset_ += incr;
        modified_ = true;
        printParam();
    }
    void tweakGain(float incr){
        gain_ += incr;
        modified_ = true;
        printParam();
    }
    void tweakDensity(float incr){
        density_ += incr;
        modified_ = true;
        printParam();
    }

    float * getParams(){
        static float params[10];
        params[0] = density_;
        params[1] = float(tex_size_);
        params[2] = H_;
        params[3] = lacunarity_;
        params[4] = octaves_;
        params[5] = offset_;
        params[6] = gain_;
        params[7] = displacement_.x;
        params[8] = displacement_.y;
        return params;
    }

    vec2 getDisplacement() const{
        return displacement_;
    }
};
