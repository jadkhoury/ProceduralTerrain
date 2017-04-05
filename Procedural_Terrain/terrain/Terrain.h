#pragma once

#include "glm/gtc/type_ptr.hpp"

#include "icg_helper.h"

#include "../utility/Utility.h"
#include "../utility/Light.h"
#include "../utility/Transformations.h"
#include "../utility/Camera.h"
#include "../utility/Grid.h"

#define ABOVE_WATER 1
#define UNDER_WATER -1
#define DONT_CLIP 0

class Terrain : Light, Grid, Transformations {

private:
    GLuint program_id_;             // GLSL shader program ID
    GLuint texture_id_;             // texture ID
    GLuint texture_noise_id_;
    GLuint texture_sand_id_, texture_grass_id_, texture_snow_id_, texture_rock_id_, tex_normal_id_;
    GLuint tex_shadows_id_;
    Transformations* transfo_;
    Light* light_;
    int shadow_map_ = 0;


public:
    void init(Transformations* transformations, Light* light, GLuint tex_shadows_in, float size, int dim, bool tess, int shadow_map) {
        grid_dim_ = dim;
        grid_size_ = size;
        tessellate_ = tess;
        transfo_ = transformations;
        light_ = light;
        shadow_map_ = shadow_map;

        if(tessellate_){
            program_id_ = icg_helper::LoadShaders("terrain_vshader.glsl",
                                                  "terrain_fshader.glsl",
                                                  "terrain_tcontrol.glsl",
                                                  "terrain_tevaluation.glsl",
                                                  "terrain_gshader.glsl");
        } else if(shadow_map_ > 0){

            program_id_ = icg_helper::LoadShaders("terrain_vshader_for_shadow.glsl",
                                                  "terrain_fshader_for_shadow.glsl");
        } else {
            program_id_ = icg_helper::LoadShaders("terrain_vshader_noTess.glsl",
                                                  "terrain_fshader_noTess.glsl");
        }
        if(!program_id_) {
            exit(EXIT_FAILURE);
        }

        glUseProgram(program_id_);

        // vertex one vertex Array
        glGenVertexArrays(1, &vertex_array_id_);
        glBindVertexArray(vertex_array_id_);

        generateGrid();

        // position buffer
        glGenBuffers(1, &vertex_buffer_object_position_);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_position_);
        glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(GLfloat),
                     &vertices_[0], GL_STATIC_DRAW);

        // vertex indices
        glGenBuffers(1, &vertex_buffer_object_index_);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertex_buffer_object_index_);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(GLuint),
                     &indices_[0], GL_STATIC_DRAW);

        // position shader attribute
        GLuint loc_position = glGetAttribLocation(program_id_, "vpoint");
        glEnableVertexAttribArray(loc_position);
        glVertexAttribPointer(loc_position, 2, GL_FLOAT, DONT_NORMALIZE,
                              ZERO_STRIDE, ZERO_BUFFER_OFFSET);



        if(shadow_map_ == 0){
            utility::loadTextureFromFile("sand.bmp", &texture_sand_id_, MIPMAP);
            utility::setUniformInt(program_id_, "tex_sand", 1);
            utility::loadTextureFromFile("grass.tga", &texture_grass_id_, MIPMAP);
            utility::setUniformInt(program_id_, "tex_grass", 2);
            utility::loadTextureFromFile("snow.bmp", &texture_snow_id_, MIPMAP);
            utility::setUniformInt(program_id_, "tex_snow", 3);
            utility::loadTextureFromFile("rock.bmp", &texture_rock_id_, MIPMAP);
            utility::setUniformInt(program_id_, "tex_rock", 4);
            tex_shadows_id_ = tex_shadows_in;
            utility::setUniformInt(program_id_, "tex_shadows", 5);
        }
        //        utility::loadTextureFromFile("mountain_h.tga", &tex_normal_id_, STANDARD);
        //        utility::setUniformInt(program_id_, "tex_normal", 5);

        glBindVertexArray(0);
        glUseProgram(0);
        cout << "--Terrain initialized" << endl;
    }

    void cleanup() {
        glBindVertexArray(0);
        glUseProgram(0);
        glDeleteBuffers(1, &vertex_buffer_object_position_);
        glDeleteBuffers(1, &vertex_buffer_object_index_);
        glDeleteProgram(program_id_);
        glDeleteVertexArrays(1, &vertex_array_id_);
        glDeleteTextures(1, &texture_id_);
        glDeleteTextures(1, &texture_noise_id_);
        glDeleteTextures(1, &texture_grass_id_);
        glDeleteTextures(1, &texture_snow_id_);
        glDeleteTextures(1, &texture_sand_id_);
        glDeleteTextures(1, &texture_rock_id_);
    }

    void draw(int clip_coef = 0, vec2 ss = vec2(1400, 800), GLuint tex_noise = -1, int shadow_but_normal_mvp = -1) {
        glUseProgram(program_id_);
        glBindVertexArray(vertex_array_id_);

        texture_noise_id_ = tex_noise;
        utility::setUniformInt(program_id_, "tex_noise", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_noise_id_);

        Grid::uploadParamToGPU(program_id_);

        if(shadow_map_ == 0) {
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, texture_sand_id_);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, texture_grass_id_);
            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, texture_snow_id_);
            glActiveTexture(GL_TEXTURE4);
            glBindTexture(GL_TEXTURE_2D, texture_rock_id_);
            glActiveTexture(GL_TEXTURE5);
            glBindTexture(GL_TEXTURE_2D, tex_shadows_id_);
            //        glActiveTexture(GL_TEXTURE5);
            //        glBindTexture(GL_TEXTURE_2D, tex_normal_id_);
        }

        if (clip_coef == ABOVE_WATER){
            transfo_->uploadToGPU(program_id_, MIRRORED);
        } else {
            transfo_->uploadToGPU(program_id_, STANDARD);
        }

        light_->uploadLightParams(program_id_);
        utility::setUniformInt(program_id_, "clip_coef", clip_coef);
        utility::setUniformInt(program_id_, "normal_mvp", shadow_but_normal_mvp);

        glEnable(GL_CLIP_DISTANCE0);
        glEnable(GL_CLIP_PLANE0);
        drawGrid();
        glDisable(GL_CLIP_DISTANCE0);
        glDisable(GL_CLIP_PLANE0);

        glActiveTexture(0);
        glBindVertexArray(0);
        glUseProgram(0);
    }
};
