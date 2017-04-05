#pragma once
#include "icg_helper.h"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "../utility/Utility.h"
#include "../utility/Transformations.h"
#include "../utility/Light.h"

#define MIRRORED 1
#define STANDARD 0
using namespace glm;

const float PI = 3.14159265359f;

static const unsigned int nb_sky_vertices = 36;
static const vec3 sky_vertices[] = {
    vec3(-0.5, -0.5, -0.5),
    vec3(-0.5, 0.5, -0.5),
    vec3(0.5, -0.5, -0.5),
    vec3(-0.5, 0.5, -0.5),
    vec3(0.5, -0.5, -0.5),
    vec3(0.5, 0.5, -0.5),
    vec3(0.5, 0.5, 0.5),
    vec3(0.5, -0.5, 0.5),
    vec3(0.5, 0.5, -0.5),
    vec3(0.5, -0.5, 0.5),
    vec3(0.5, 0.5, -0.5),
    vec3(0.5, -0.5, -0.5),
    vec3(0.5, 0.5, 0.5),
    vec3(-0.5, 0.5, 0.5),
    vec3(0.5, -0.5, 0.5),
    vec3(-0.5, 0.5, 0.5),
    vec3(0.5, -0.5, 0.5),
    vec3(-0.5, -0.5, 0.5),
    vec3(-0.5, -0.5, 0.5),
    vec3(-0.5, -0.5, -0.5),
    vec3(0.5, -0.5, 0.5),
    vec3(-0.5, -0.5, -0.5),
    vec3(0.5, -0.5, 0.5),
    vec3(0.5, -0.5, -0.5),
    vec3(-0.5, 0.5, -0.5),
    vec3(-0.5, -0.5, -0.5),
    vec3(-0.5, 0.5, 0.5),
    vec3(-0.5, -0.5, -0.5),
    vec3(-0.5, 0.5, 0.5),
    vec3(-0.5, -0.5, 0.5),
    vec3(0.5, 0.5, -0.5),
    vec3(-0.5, 0.5, -0.5),
    vec3(0.5, 0.5, 0.5),
    vec3(-0.5, 0.5, -0.5),
    vec3(0.5, 0.5, 0.5),
    vec3(-0.5, 0.5, 0.5)
};

static const unsigned int nb_sky_colors = 36;
static const  vec3 sky_colors[] = {
    vec3(0.583f,  0.771f,  0.014f),
    vec3(0.609f,  0.115f,  0.436f),
    vec3(0.327f,  0.483f,  0.844f),
    vec3(0.822f,  0.569f,  0.201f),
    vec3(0.435f,  0.602f,  0.223f),
    vec3(0.310f,  0.747f,  0.185f),
    vec3(0.597f,  0.770f,  0.761f),
    vec3(0.559f,  0.436f,  0.730f),
    vec3(0.359f,  0.583f,  0.152f),
    vec3(0.483f,  0.596f,  0.789f),
    vec3(0.559f,  0.861f,  0.639f),
    vec3(0.195f,  0.548f,  0.859f),
    vec3(0.014f,  0.184f,  0.576f),
    vec3(0.771f,  0.328f,  0.970f),
    vec3(0.406f,  0.615f,  0.116f),
    vec3(0.676f,  0.977f,  0.133f),
    vec3(0.971f,  0.572f,  0.833f),
    vec3(0.140f,  0.616f,  0.489f),
    vec3(0.997f,  0.513f,  0.064f),
    vec3(0.945f,  0.719f,  0.592f),
    vec3(0.543f,  0.021f,  0.978f),
    vec3(0.279f,  0.317f,  0.505f),
    vec3(0.167f,  0.620f,  0.077f),
    vec3(0.347f,  0.857f,  0.137f),
    vec3(0.055f,  0.953f,  0.042f),
    vec3(0.714f,  0.505f,  0.345f),
    vec3(0.783f,  0.290f,  0.734f),
    vec3(0.722f,  0.645f,  0.174f),
    vec3(0.302f,  0.455f,  0.848f),
    vec3(0.225f,  0.587f,  0.040f),
    vec3(0.517f,  0.713f,  0.338f),
    vec3(0.053f,  0.959f,  0.120f),
    vec3(0.393f,  0.621f,  0.362f),
    vec3(0.673f,  0.211f,  0.457f),
    vec3(0.820f,  0.883f,  0.371f),
    vec3(0.982f,  0.099f,  0.879f)
};

static const unsigned int nb_sky_UVs = 36;
static const  vec2 sky_UVs[] = {
    vec2(0.333, 0.75),
    vec2(0.666, 0.75),
    vec2(0.333, 0.5),
    vec2(0.666, 0.75),
    vec2(0.333, 0.5),
    vec2(0.666, 0.5),

    vec2(0.666, 0.25),
    vec2(0.333, 0.25),
    vec2(0.666, 0.5),
    vec2(0.333, 0.25),
    vec2(0.666, 0.5),
    vec2(0.333, 0.5),

    vec2(0.666, 0.25),
    vec2(0.666, 0.0),
    vec2(0.333, 0.25),
    vec2(0.666, 0.0),
    vec2(0.333, 0.25),
    vec2(0.333, 0.0),

    vec2(0.0, 0.75),
    vec2(0.333, 0.75),
    vec2(0.0, 0.5),
    vec2(0.333, 0.75),
    vec2(0.0, 0.5),
    vec2(0.333, 0.5),

    vec2(0.666, 0.75),
    vec2(0.333, 0.75),
    vec2(0.666, 1.0),
    vec2(0.333, 0.75),
    vec2(0.666, 1.0),
    vec2(0.333, 1.0),

    vec2(0.666, 0.5),
    vec2(0.666, 0.75),
    vec2(1.0, 0.5),
    vec2(0.666, 0.75),
    vec2(1.0, 0.5),
    vec2(1.0, 0.75)
};

class Sky {

private:
    GLuint vertex_array_id_;        // vertex array object
    GLuint program_id_;             // GLSL shader program ID
    GLuint vertex_buffer_object_;   // memory buffer
    GLuint texture_day_id_;             // texture ID
    GLuint texture_night_id_;
    mat4 model_matrix_;             // model matrix
    Transformations* transfo_;
    Light* light_;

public:
    void init(Transformations* transformations, Light* light) {
        // compile the shaders.
        program_id_ = icg_helper::LoadShaders("sky_vshader.glsl",
                                              "sky_fshader.glsl");
        if(!program_id_) {
            exit(EXIT_FAILURE);
        }

        glUseProgram(program_id_);

        // vertex one vertex array
        glGenVertexArrays(1, &vertex_array_id_);
        glBindVertexArray(vertex_array_id_);

        // vertex coordinates
        {
            // buffer
            glGenBuffers(1, &vertex_buffer_object_);
            glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_);
            glBufferData(GL_ARRAY_BUFFER, nb_sky_vertices * sizeof( vec3),
                         &sky_vertices[0], GL_STATIC_DRAW);

            // attribute
            GLuint vertex_point_id = glGetAttribLocation(program_id_, "vpoint");
            glEnableVertexAttribArray(vertex_point_id);
            glVertexAttribPointer(vertex_point_id, 3, GL_FLOAT, DONT_NORMALIZE,
                                  ZERO_STRIDE, ZERO_BUFFER_OFFSET);
        }

        // texture coordinates
        {
            // buffer
            glGenBuffers(1, &vertex_buffer_object_);
            glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_);
            glBufferData(GL_ARRAY_BUFFER, nb_sky_UVs * sizeof( vec2),
                         &sky_UVs[0], GL_STATIC_DRAW);

            // attribute
            GLuint vertex_texture_coord_id = glGetAttribLocation(program_id_, "vtexcoord");
            glEnableVertexAttribArray(vertex_texture_coord_id);
            glVertexAttribPointer(vertex_texture_coord_id, 2, GL_FLOAT,
                                  DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);
        }
        utility::loadTextureFromFile("sky_texture_night.bmp", &texture_night_id_, STANDARD);
        utility::setUniformInt(program_id_, "tex_day", 0);
        utility::loadTextureFromFile("sky_texture.tga", &texture_day_id_, STANDARD);
        utility::setUniformInt(program_id_, "tex_night", 1);

        // create the model matrix
        model_matrix_= IDENTITY_MATRIX;
        model_matrix_ = rotate(model_matrix_, PI/2.0f, vec3(1.0, 0.0, 0.0));
        model_matrix_ = scale(model_matrix_,  vec3(100.0f));

        transfo_ = transformations;
        light_ = light;

        glBindVertexArray(0);
        glUseProgram(0);
        cout << "--Sky initialized" << endl;

    }

    void cleanup() {
        glBindVertexArray(0);
        glUseProgram(0);
        glDeleteBuffers(1, &vertex_buffer_object_);
        glDeleteProgram(program_id_);
        glDeleteVertexArrays(1, &vertex_array_id_);
        glDeleteTextures(1, &texture_day_id_);
        glDeleteTextures(1, &texture_night_id_);
    }

    void draw(bool mirrored){
        glUseProgram(program_id_);
        glBindVertexArray(vertex_array_id_);

        // bind textures
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_day_id_);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture_night_id_);

        mat4 MVP;
        // setup MVP
        if (mirrored > 0){
            MVP = transfo_->MVP_mirrored()* model_matrix_;
        } else {
            MVP = transfo_->MVP()*translate(IDENTITY_MATRIX, transfo_->camPos() - vec3(0.0, 0.0, 2.0)) * model_matrix_;
        }
        light_->uploadLightParams(program_id_);

        utility::setUniformMat4(program_id_, "MVP", MVP);
        // draw
        glDrawArrays(GL_TRIANGLES,0, nb_sky_vertices);

        glBindVertexArray(0);
        glUseProgram(0);
    }
};
