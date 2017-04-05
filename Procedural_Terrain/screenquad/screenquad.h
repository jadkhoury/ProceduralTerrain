#pragma once
#include "icg_helper.h"
#include "../utility/Utility.h"
#include "../utility/Transformations.h"

class ScreenQuad {

private:
    GLuint vertex_array_id_;
    GLuint program_id_;
    GLuint vertex_buffer_object_;
    GLuint texture_id_;

    Transformations* transfo_;
    float std_dev = 2;
    int fast = 1;

    int screenquad_width_;
    int screenquad_height_;

public:
    void Init(Transformations* transfo, float render_size, int width, int height, GLuint screen_tex) {

        // set screenquad size
        this->screenquad_width_ = width;
        this->screenquad_height_ = height;

        // compile the shaders
        program_id_ = icg_helper::LoadShaders("screenquad_vshader.glsl",
                                              "screenquad_fshader.glsl");
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
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_point),
                         vertex_point, GL_STATIC_DRAW);

            // attribute
            GLuint vertex_point_id = glGetAttribLocation(program_id_, "vpoint");
            glEnableVertexAttribArray(vertex_point_id);
            glVertexAttribPointer(vertex_point_id, 3, GL_FLOAT, DONT_NORMALIZE,
                                  ZERO_STRIDE, ZERO_BUFFER_OFFSET);
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
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_texture_coordinates),
                         vertex_texture_coordinates, GL_STATIC_DRAW);

            // attribute
            GLuint vertex_texture_coord_id = glGetAttribLocation(program_id_,
                                                                 "vtexcoord");
            glEnableVertexAttribArray(vertex_texture_coord_id);
            glVertexAttribPointer(vertex_texture_coord_id, 2, GL_FLOAT,
                                  DONT_NORMALIZE, ZERO_STRIDE,
                                  ZERO_BUFFER_OFFSET);
        }

        utility::setUniformInt(program_id_, "grid_size", render_size);
        // load/Assign texture

        this->texture_id_ = screen_tex;
        glBindTexture(GL_TEXTURE_2D, texture_id_);
        glUniform1i(glGetUniformLocation(program_id_, "tex"),
                    0 /*GL_TEXTURE0*/);

        transfo_ = transfo;
        // to avoid the current object being polluted
        glBindVertexArray(0);
        glUseProgram(0);
    }

    void Cleanup() {
        glBindVertexArray(0);
        glUseProgram(0);
        glDeleteBuffers(1, &vertex_buffer_object_);
        glDeleteProgram(program_id_);
        glDeleteVertexArrays(1, &vertex_array_id_);
        glDeleteTextures(1, &texture_id_);
    }

    void UpdateSize(int screenquad_width, int screenquad_height) {
        this->screenquad_width_ = screenquad_width;
        this->screenquad_height_ = screenquad_height;
    }


    void draw() {
        glUseProgram(program_id_);
        glBindVertexArray(vertex_array_id_);

        glUniform1f(glGetUniformLocation(program_id_, "std"), std_dev);
        glUniform1f(glGetUniformLocation(program_id_, "tex_width"),
                    screenquad_width_);
        glUniform1f(glGetUniformLocation(program_id_, "tex_height"),
                    screenquad_height_);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_id_);

        transfo_->uploadScreenQuadParam(program_id_, STANDARD);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glBindVertexArray(0);
        glUseProgram(0);
    }
};
