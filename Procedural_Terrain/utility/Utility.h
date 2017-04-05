#pragma once

#include "glm/gtc/type_ptr.hpp"

#include <sstream>
#include <string>

#include "icg_helper.h"


#define MIPMAP 1
#define STANDARD 0

using namespace glm;

namespace utility {
static void setUniformInt(GLuint pid, string name, int val){
    const char* c = name.c_str();
    GLuint location = glGetUniformLocation(pid, c);
    glUniform1i(location, val);
}

static void setUniformFloat(GLuint pid, string name, float val){
    const char* c = name.c_str();
    GLuint location = glGetUniformLocation(pid, c);
    glUniform1f(location, val);
}

static void setUniformMat4(GLuint pid, string name, const mat4& value){
    const char* c = name.c_str();
    GLuint location = glGetUniformLocation(pid, c);
    glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(value));
}

static void setUniformVec2(GLuint pid, string name, const vec2& value){
    const char* c = name.c_str();
    GLuint location = glGetUniformLocation(pid, c);
    glUniform2fv(location, 1, value_ptr(value));
}

static void setUniformVec3(GLuint pid, string name, const vec3& value){
    const char* c = name.c_str();
    GLuint location = glGetUniformLocation(pid, c);
    glUniform3fv(location, 1, value_ptr(value));
}

static void loadTextureFromFile(string filename, GLuint* id, bool mipmap){
    // load texture
    int width;
    int height;
    int nb_component;
    // set stb_image to have the same coordinates as OpenGL
    stbi_set_flip_vertically_on_load(1);
    unsigned char* image = stbi_load(filename.c_str(), &width,
                                     &height, &nb_component, 0);
    if(image == nullptr) {
        throw(string("Failed to load texture"));
    }
    glGenTextures(1, id);
    glBindTexture(GL_TEXTURE_2D, *id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    if(mipmap > 0) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        //        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_LOD, 0.0);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_LOD, 3.0);
    } else
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    if(nb_component == 3) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
                     GL_RGB, GL_UNSIGNED_BYTE, image);
    } else if(nb_component == 4) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
                     GL_RGBA, GL_UNSIGNED_BYTE, image);
    }
    if(mipmap>0)
        glGenerateMipmap(GL_TEXTURE_2D);
    // cleanup
    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(image);
}
string convertInt(int number){
    stringstream ss;
    ss << number;
    return ss.str();
}
int ScreenShot(int m, string name = ""){
    unsigned char *pixels;
    FILE * shot;

    int screenStats[4];
    glGetIntegerv(GL_VIEWPORT, screenStats);
    const unsigned int WIDTH = screenStats[2];
    const unsigned int HEIGHT = screenStats[3];
    const unsigned char w0 = (int)(WIDTH%256);
    const unsigned char w1 = (int)(WIDTH/256);
    const unsigned char h0 = (int)(HEIGHT%256);
    const unsigned char h1 = (int)(HEIGHT/256);

    pixels = new unsigned char[WIDTH*HEIGHT*3];
    glReadPixels(0, 0, WIDTH, HEIGHT, GL_BGR, GL_UNSIGNED_BYTE, pixels);

    string buffer;
    if(name ==""){
        if (m < 10)
            buffer = "00" + convertInt(m) + ".tga";
        else if (m < 100)
            buffer = "0" + convertInt(m) + ".tga";
        else
            buffer = convertInt(m) + ".tga";
    } else {
        if (m < 10)
            buffer = name + "00" + convertInt(m) + ".tga";
        else if (m < 100)
            buffer = name + "0" + convertInt(m) + ".tga";
        else
            buffer = name + convertInt(m) + ".tga";
    }

    if((shot=fopen(buffer.c_str(), "wb"))==NULL) return 1;

    unsigned char TGAheader[12]={0,0,2,0,0,0,0,0,0,0,0,0};
    unsigned char header[6]={w0, w1, h0, h1, 24, 0};

    fwrite(TGAheader, sizeof(unsigned char), 12, shot);
    fwrite(header, sizeof(unsigned char), 6, shot);
    fwrite(pixels, sizeof(unsigned char), WIDTH*HEIGHT*3, shot);

    fclose(shot);
    delete [] pixels;

    //cout << "Image saved as: " << buffer << endl;

    return 0;
}

}
