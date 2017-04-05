#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <string>

#include "main.h"

#include "utility/Utility.h"
#include "utility/FrameBuffer.h"
#include "utility/Camera.h"
#include "utility/CameraBezierPath.h"
#include "utility/Controls.h"
#include "utility/Transformations.h"

#include "water/Water.h"
#include <unistd.h>

#include "sky/Sky.h"
#include "noise/Noise.h"
#include "terrain/Terrain.h"
#include "cube/Cube.h"
#include "quad/quad.h"
#include "utility/Light.h"
#include "utility/Height.h"

#include"screenquad/screenquad.h"

using namespace std;

double last_time;
int nb_frames = 0;

int window_width_ = 1920    ;
int window_height_ = 1080;
float render_size = 25.0;
int tex_size = 2048;

//Buffers and their tex
FrameBuffer height_buffer_, height_buffer2_;
FrameBuffer reflection_buffer_;
FrameBuffer refraction_buffer_;
FrameBuffer light_buffer_;
FrameBuffer screen_buffer_;
FrameBuffer shadow_buffer_;
GLuint fb_tex_noise_, fb_tex_noise_2_, current_noise_buffer_;
GLuint fb_above_water_;
GLuint fb_under_water_;
GLuint fb_tex_light_;
GLuint fb_tex_screen_;
GLuint fb_tex_shadows_;
//Objects
ScreenQuad screen_quad_;
Sky sky_;
Water water_;
Terrain terrain_;
Terrain terrain_for_shadows;
Cube cube_;
Noise noise_;
Quad quad_;
//Utilities
Camera cam_;
Controls controls;
CameraBezierPath camera_path;
bool cam_demo_enabled = false;
Transformations transformations_;
Light light_;
GLFWwindow* window_;
string str = "Terrain Rendering";
const char* window_title_ = str.c_str();

vec2 transformScreenCoords(GLFWwindow* window, int x, int y) {
    int width;
    int height;
    glfwGetWindowSize(window, &width, &height);
    return vec2(2.0f * (float)x / width - 1.0f,
                1.0f - 2.0f * (float)y / height);
}

void initWindow(){
    /// Hint GLFW that we would like an OpenGL 3 context (at least)
    /// http://www.glfw.org/faq.html#how-do-i-create-an-opengl-30-context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

void errorCallback(int error, const char* description) {
    fputs(description, stderr);
}

void initViewport(int width, int height){
    glViewport(0 , 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

// Gets called when the windows/framebuffer is resized.
void resizeCallback(GLFWwindow* window, int width, int height) {
    glfwGetFramebufferSize(window, &window_width_, &window_height_);
    transformations_.updateProjection(45.0, 0.1f, window_width_, window_height_);
    initViewport(window_width_, window_height_);
    refraction_buffer_.cleanup();
    refraction_buffer_.init(window_width_, window_height_, GL_RGB8, 1);
    reflection_buffer_.cleanup();
    reflection_buffer_.init(window_width_, window_height_, GL_RGB8, 1);
    screen_buffer_.cleanup();
    screen_buffer_.init(window_width_, window_height_, GL_RGB8, 1);
    screen_quad_.UpdateSize(window_width_, window_height_);


}

void setTitleWindow(GLFWwindow *pWindow, string title){
    glfwSetWindowTitle(pWindow, "test");
    return;

    // Measure speed
    double current_time = glfwGetTime();
    double delta = current_time - last_time;
    nb_frames++;
    if ( delta >= 1.0 ){ // If last cout was more than 1 sec ago
        double fps = double(nb_frames) / delta;
        stringstream ss;
        ss << title << " "  << " [" << fps << " FPS]";
        glfwSetWindowTitle(pWindow, ss.str().c_str());
        nb_frames = 0;
        last_time = current_time;
    }
}

int initGlfw(){
    if( !glfwInit() ){
        fprintf( stderr, "Failed to initialize GLFW\n" );
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

GLFWwindow* openWindow(string title){
    /// Attempt to open the window: fails if required version unavailable
    /// @note some Intel GPUs do not support OpenGL 3.2
    /// @note update the driver of your graphic card
    const char* c = title.c_str();
    window_ = glfwCreateWindow(window_width_, window_height_, c, NULL, NULL);
    if( !window_ ){
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    return window_;
}

void initGlew(){
    /// GLEW Initialization (must have a context)
    glewExperimental = GL_TRUE; ///<
    if( glewInit() != GLEW_OK ){
        fprintf( stderr, "Failed to initialize GLEW\n");
        exit(EXIT_FAILURE);
    }
    glGetError();
}

void closeWindow(GLFWwindow* window){
    /// Close OpenGL window and terminate GLFW
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}

