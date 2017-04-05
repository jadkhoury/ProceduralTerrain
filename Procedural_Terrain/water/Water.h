#pragma once

#include "glm/gtc/type_ptr.hpp"

#include "icg_helper.h"

#include "../utility/Grid.h"
#include "../utility/Light.h"
#include "../utility/Utility.h"
#include "../utility/Transformations.h"



class Water : Light, Grid {

private:
    // memory buffer for indices
    GLuint program_id_;             // GLSL shader program ID
    GLuint normal_map_id_;
    GLuint dudv_map_id_;    // texture ID
    GLuint tex_reflection_id_;
    GLuint tex_refraction_id_;
    GLuint texture_noise_id_;
    GLuint texture_shadow_id_;// texture mirror ID
    GLuint MVP_id_;                 // model, view, proj matrix ID
    Transformations* transfo_;
    Light* light_;

public:
    void init(float size, int dim, Transformations* transformations, Light* light, GLuint tex_reflection = -1, GLuint tex_refraction = -1, GLuint tex_shadow=-1) {
        grid_dim_ = dim;
        grid_size_ = size;
        tessellate_ = false;
        light_ = light;

        // compile the shaders
        program_id_ = icg_helper::LoadShaders("water_vshader.glsl",
                                              "water_fshader.glsl");
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

        utility::loadTextureFromFile("water4.bmp", &normal_map_id_, STANDARD);
        utility::setUniformInt(program_id_, "tex", 0);


        tex_reflection_id_ = (tex_reflection==-1)? normal_map_id_ : tex_reflection;
        utility::setUniformInt(program_id_, "tex_mirror", 1);

        tex_refraction_id_ = (tex_reflection==-1)? normal_map_id_ : tex_refraction;
        utility::setUniformInt(program_id_, "tex_refract", 2);

        utility::loadTextureFromFile("water_dudv.bmp", &dudv_map_id_, STANDARD);
        utility::setUniformInt(program_id_, "tex_dudv", 4);

        texture_shadow_id_ = tex_shadow;
        utility::setUniformInt(program_id_, "tex_shadows", 5);


        MVP_id_ = glGetUniformLocation(program_id_, "MVP");
        utility::setUniformFloat(program_id_, "grid_size", grid_size_);
        utility::setUniformInt(program_id_, "grid_dim", grid_dim_);

        //Camera pointer
        transfo_ = transformations;

        // to avoid the current object being polluted
        glBindVertexArray(0);
        glUseProgram(0);
        cout << "--Water initialized" << endl;
    }

    void cleanup() {
        glBindVertexArray(0);
        glUseProgram(0);
        glDeleteBuffers(1, &vertex_buffer_object_position_);
        glDeleteBuffers(1, &vertex_buffer_object_index_);
        glDeleteProgram(program_id_);
        glDeleteVertexArrays(1, &vertex_array_id_);
        glDeleteTextures(1, &normal_map_id_);
        glDeleteTextures(1, &tex_reflection_id_);
        glDeleteTextures(1, &tex_refraction_id_);
    }

    void draw(GLuint tex_noise) {
        glUseProgram(program_id_);
        glBindVertexArray(vertex_array_id_);
        // setup MVP
        transfo_->uploadToGPU(program_id_, STANDARD);
        light_->uploadLightParams(program_id_);


        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, normal_map_id_);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, tex_reflection_id_);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, tex_refraction_id_);

        texture_noise_id_ = tex_noise;
        utility::setUniformInt(program_id_, "tex_noise", 3);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, texture_noise_id_);

        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, dudv_map_id_);

        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, texture_shadow_id_);

        // draw
        drawGrid();

        glBindVertexArray(0);
        glUseProgram(0);
    }
};
