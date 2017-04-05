#pragma once

#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "icg_helper.h"

using namespace glm;

static const unsigned int nb_cube_vertices = 36;
static const vec3 cube_vertices[] = {
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

static const unsigned int nb_cube_colors = 36;
static const  vec3 cube_colors[] = {
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

static const unsigned int nb_cube_UVs = 36;
static const  vec2 cube_UVs[] = {
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

class Cube {

    private:
        GLuint vertex_array_id_;        // vertex array object
        GLuint program_id_;             // GLSL shader program ID
        GLuint vertex_buffer_object_;   // memory buffer
        GLuint texture_id_;             // texture ID
        mat4 model_matrix_;             // model matrix

    public:
        void init() {
            // compile the shaders.
            program_id_ = icg_helper::LoadShaders("cube_vshader.glsl",
                                                  "cube_fshader.glsl");
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
                glBufferData(GL_ARRAY_BUFFER, nb_cube_vertices * sizeof( vec3),
                             &cube_vertices[0], GL_STATIC_DRAW);

                // attribute
                GLuint vertex_point_id = glGetAttribLocation(program_id_, "vpoint");
                glEnableVertexAttribArray(vertex_point_id);
                glVertexAttribPointer(vertex_point_id, 3, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);
            }

            // texture coordinates
            {
                // buffer
                glGenBuffers(1, &vertex_buffer_object_);
                glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_);
                glBufferData(GL_ARRAY_BUFFER, nb_cube_UVs * sizeof( vec2), &cube_UVs[0], GL_STATIC_DRAW);

                // attribute
                GLuint vertex_texture_coord_id = glGetAttribLocation(program_id_, "vtexcoord");
                glEnableVertexAttribArray(vertex_texture_coord_id);
                glVertexAttribPointer(vertex_texture_coord_id, 2, GL_FLOAT,
                                      DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);
            }

            // load texture
            {
                int width;
                int height;
                int nb_component;
                string texture_filename = "cube_texture.tga";
                stbi_set_flip_vertically_on_load(1);
                unsigned char* image = stbi_load(texture_filename.c_str(),
                                                 &width, &height, &nb_component, 0);

                if(image == nullptr) {
                    throw(std::string("Failed to load texture"));
                }

                glGenTextures(1, &texture_id_);
                glBindTexture(GL_TEXTURE_2D, texture_id_);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

                if(nb_component == 3) {
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
                                 GL_RGB, GL_UNSIGNED_BYTE, image);
                } else if(nb_component == 4) {
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
                                 GL_RGBA, GL_UNSIGNED_BYTE, image);
                }

                GLuint tex_id = glGetUniformLocation(program_id_, "tex");
                glUniform1i(tex_id, 0 /*GL_TEXTURE0*/);

                // cleanup
                glBindTexture(GL_TEXTURE_2D, 0);
                stbi_image_free(image);

            }

            // create the model matrix
            model_matrix_ = scale(IDENTITY_MATRIX, vec3(0.3f));
            model_matrix_ = translate(model_matrix_, vec3(0.0f, 0.0f, 3.0f));
            glBindVertexArray(0);
            glUseProgram(0);
        }

        void cleanup() {
            glBindVertexArray(0);
            glUseProgram(0);
            glDeleteBuffers(1, &vertex_buffer_object_);
            glDeleteProgram(program_id_);
            glDeleteVertexArrays(1, &vertex_array_id_);
            glDeleteTextures(1, &texture_id_);
        }

        void draw(const  mat4& view_projection){
            glUseProgram(program_id_);
            glBindVertexArray(vertex_array_id_);

            // bind textures
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture_id_);

            // time
            glUniform1f(glGetUniformLocation(program_id_, "time"), glfwGetTime());

            // setup MVP
            mat4 MVP = view_projection * model_matrix_;
            GLuint MVP_id = glGetUniformLocation(program_id_, "MVP");
            glUniformMatrix4fv(MVP_id, 1, GL_FALSE, value_ptr(MVP));

            // draw
            glDrawArrays(GL_TRIANGLES,0, nb_cube_vertices);

            glBindVertexArray(0);
            glUseProgram(0);
        }
};
