#pragma once

#define DONT_TESSELLATE 0
#define TESSELLATE 1

#include "glm/gtc/type_ptr.hpp"

#include "icg_helper.h"
#include "Utility.h"

using namespace glm;

class Grid{
    
    protected:
        GLuint vertex_array_id_;                // vertex array object
        GLuint vertex_buffer_object_position_;  // memory buffer for positions
        GLuint vertex_buffer_object_index_;     // memory buffer for indices
        std::vector<GLfloat> vertices_;
        std::vector<GLuint> indices_;
        GLuint num_indices_;                    // number of vertices to render

        float grid_size_;
        float grid_dim_;
        bool tessellate_;

        //Generate a grid with 2 different algorithms,
        //If tessellation is needed, the patches must be provided
        void generateGrid(){
            float length = grid_size_ / (grid_dim_ - 1.0);
            float left = - grid_size_ / 2.0;
            for (int row = 0; row <= grid_dim_ - 1; ++row) {
                for (int col = 0; col <= grid_dim_ - 1; ++col) {
                    vertices_.push_back(left + row * length);
                    vertices_.push_back(left + col * length);
                }
            }
            if(tessellate_){ //Indices for the patches of tessellation
                for (int row = 0; row < grid_dim_ - 1; ++row) {
                    for (int col = 0; col < grid_dim_ - 1; ++col) {
                        indices_.push_back((grid_dim_) * row + col);
                        indices_.push_back((grid_dim_) * (row + 1) + col);
                        indices_.push_back((grid_dim_) * row + (col + 1));
                        indices_.push_back((grid_dim_) * (row + 1) + (col + 1));
                    }
                }
            } else { //Indices for the triangle_strip
                for(int row = 0; row <= grid_dim_ - 2; row++) {
                    if (row % 2 == 0){
                        for(int col = 0; col <= (grid_dim_ - 1); col++) {
                            indices_.push_back(row * grid_dim_ + col);
                            indices_.push_back((row+1) * grid_dim_ + col);
                        }
                    } else {
                        for (int col = (grid_dim_ - 1); col >= 0; col--){
                            indices_.push_back(row * grid_dim_ + col);
                            indices_.push_back((row + 1) * grid_dim_ + col);
                        }
                    }
                }
            }
            num_indices_ = indices_.size();
        }

        void drawGrid(){
            if (tessellate_){
                glPatchParameteri(GL_PATCH_VERTICES, 4);
                glDrawElements(GL_PATCHES, num_indices_, GL_UNSIGNED_INT, 0);
            } else {
                glDrawElements(GL_TRIANGLE_STRIP, num_indices_, GL_UNSIGNED_INT, 0);
            }
        }

        void uploadParamToGPU(GLuint pid){
            utility::setUniformFloat(pid, "grid_size", grid_size_);
            utility::setUniformInt(pid, "grid_dim", grid_dim_);
        }
};
