#include "main.h"

// We need to take into account the last buffer we wrote into
// If last = 1, noise read from 1 and write in 2, terrain read 2
// If last = 2, noise read from 2 and write in 1, terrain read 1
int last_buffer = 0;
double total_time = 0.0;
double delta_time = 0.0;

void init(GLFWwindow* window) {
    glClearColor(1.0, 1.0, 1.0 /*white*/, 1.0 /*solid*/);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);

    cam_ = Camera();
    controls = Controls(&cam_, &noise_);

    glfwGetFramebufferSize(window, &window_width_, &window_height_);
    transformations_.updateProjection(45.0f, 0.1f, window_width_, window_height_);
    transformations_.updateCamPos(cam_.getPosition());

    fb_tex_noise_ = height_buffer_.init(tex_size, tex_size, GL_R32F, 1);
    fb_tex_noise_2_ = height_buffer2_.init(tex_size, tex_size, GL_R32F, 1);
    fb_tex_screen_ = screen_buffer_.init(window_width_, window_height_, GL_RGB8, 0);
    fb_tex_shadows_ = shadow_buffer_.init(1024, 1024, GL_DEPTH_COMPONENT16, 1);

    quad_.Init(&transformations_, &light_);
    screen_quad_.Init(&transformations_, render_size, window_width_, window_height_, fb_tex_screen_);
    noise_.init(HEIGHT_MAP, tex_size, fb_tex_noise_, fb_tex_noise_2_);
    terrain_.init(&transformations_, &light_, fb_tex_shadows_, render_size, 256, TESSELLATE, 0);
    terrain_for_shadows.init(&transformations_, &light_, -1, render_size, 1024, DONT_TESSELLATE, 1);

    height_buffer_.bind(); {
        initViewport(tex_size,tex_size);
        noise_.drawHeightMap(1);
        last_buffer = 1;
    } height_buffer_.unBind();

    fb_above_water_ = reflection_buffer_.init(window_width_, window_height_, GL_RGB8, 1);
    fb_under_water_ = refraction_buffer_.init(window_width_, window_height_, GL_RGB8, 1);

    water_.init(render_size, 10, &transformations_, &light_, fb_above_water_, fb_under_water_);
    sky_.init(&transformations_, &light_);
}
float transfotime=0, noisetime=0, reflectiontime=0, refractiontime=0, shadowtime=0, screenbuffertime=0, controltime=0, tmp=0, printtime=0, starttime=0, cummulate=0;
bool firsttime = true;
int nbFrame = 0;
int frame_nb_ = 0;
bool capture = false;
bool already_errored = false;
void display() {
    nbFrame++;
    starttime = glfwGetTime();
    tmp = glfwGetTime();

    delta_time = glfwGetTime() - total_time;
    total_time += delta_time;
    light_.updateLightPos(glfwGetTime());
    transformations_.updateTransformations(cam_.lookAt(), cam_.lookAtMirrored(), light_.getLightPos(), noise_.getDisplacement());
    transformations_.updateCamPos(cam_.getPosition());

    transfotime += glfwGetTime() - tmp;
    tmp = glfwGetTime();

    vec2 ss = vec2(window_width_, window_height_);

    // read buffer 1 write in 2
    if(last_buffer == 1){
        height_buffer2_.bind();
        initViewport(tex_size,tex_size);
        noise_.drawHeightMap(2, last_buffer);
        height_buffer2_.unBind();
        last_buffer = 2;
        current_noise_buffer_ = fb_tex_noise_2_;
        //read buffer 2 write in 1
    } else if(last_buffer == 2) {
        height_buffer_.bind();
        initViewport(tex_size,tex_size);
        noise_.drawHeightMap(2, last_buffer);
        height_buffer_.unBind();
        last_buffer = 1;
        current_noise_buffer_ = fb_tex_noise_;
    }
    noisetime += glfwGetTime() - tmp;
    tmp = glfwGetTime();


    reflection_buffer_.bind();{
        glfwGetFramebufferSize(window_, &window_width_, &window_height_);
        initViewport(window_width_, window_height_);
        sky_.draw(MIRRORED);
        terrain_.draw(ABOVE_WATER, ss, current_noise_buffer_);
    } reflection_buffer_.unBind();
    reflectiontime += glfwGetTime() - tmp;
    tmp = glfwGetTime();

    refraction_buffer_.bind();{
        glfwGetFramebufferSize(window_, &window_width_, &window_height_);
        initViewport(window_width_, window_height_);
        terrain_.draw(UNDER_WATER, ss, current_noise_buffer_);
    } refraction_buffer_.unBind();

    refractiontime += glfwGetTime() - tmp;
    tmp = glfwGetTime();

    shadow_buffer_.bind();{
        initViewport(1024, 1024);
        terrain_for_shadows.draw(DONT_CLIP, ss, current_noise_buffer_);
    } shadow_buffer_.unBind();

    shadowtime += glfwGetTime() - tmp;
    tmp = glfwGetTime();

    screen_buffer_.bind();{
        bool depth = false;;

        if(depth){
            initViewport(1024, 1024);
            terrain_for_shadows.draw(DONT_CLIP, ss, current_noise_buffer_);
        } else {
            glfwGetFramebufferSize(window_, &window_width_, &window_height_);
            initViewport(window_width_, window_height_);
            sky_.draw(STANDARD);
            terrain_.draw(DONT_CLIP, ss, current_noise_buffer_);
            water_.draw(current_noise_buffer_);
        }

    }screen_buffer_.unBind();

    initViewport(window_width_, window_height_);
    screen_quad_.draw();

    screenbuffertime += glfwGetTime() - tmp;
    tmp = glfwGetTime();



    if(cam_demo_enabled) {
        cam_demo_enabled = camera_path.move(delta_time);
    } else {
        controls.physicalMove(glfwGetKey(window_, GLFW_KEY_W) == GLFW_PRESS,
                              glfwGetKey(window_, GLFW_KEY_S) == GLFW_PRESS,
                              glfwGetKey(window_, GLFW_KEY_A) == GLFW_PRESS,
                              glfwGetKey(window_, GLFW_KEY_D) == GLFW_PRESS);
    }
    
    vec3 current_position = controls.getRealCamPos();
    //cout << "Camera position : (" << current_position.x << ", " << current_position.y << ", " << current_position.z << ")" << endl;
    vec3 look_ = cam_.getLook();
    //cout << "Camera Look: " << look_.x << ", " << look_.y << ", " << look_.z << ")" << endl;

    controltime += glfwGetTime() - tmp;
    cummulate += glfwGetTime() - starttime;
    if(firsttime  || glfwGetTime() - printtime > 5.0){
        printtime = glfwGetTime();
        firsttime = false;
        std::cout << "transfo:      " << transfotime / cummulate * 100 << "%" << endl;
        std::cout << "noise:        " << noisetime / cummulate * 100 << "%" << endl;
        std::cout << "reflection:   " << reflectiontime / cummulate * 100 << "%" << endl;
        std::cout << "refraction:   " << refractiontime / cummulate * 100 << "%" << endl;
        std::cout << "shadow:       " << shadowtime / cummulate * 100 << "%" << endl;
        std::cout << "screenbuffer: " << screenbuffertime / cummulate * 100 << "%" << endl<< endl;
        std::cout << "Average FPS over 5 sec: " << nbFrame / 5.0 << " FPS" <<endl;
        nbFrame = 0.0;
        transfotime=0, noisetime=0, reflectiontime=0, refractiontime=0, shadowtime=0, screenbuffertime=0, controltime=0, tmp=0, starttime=0, cummulate=0;

    }

    if(capture){
        if(utility::ScreenShot(frame_nb_, "../../recording/") != 0 && !already_errored){
            cout << "Unable to save frames" << endl;;
            already_errored = true;
        } else {
            frame_nb_++;
        }
    }

}

double last_x_, last_y_;
bool first_call_ = true;

void mousePos(GLFWwindow* window, double x, double y) {
    if(first_call_) {
        glfwGetWindowSize(window, &window_width_, &window_height_);
        glfwSetCursorPos(window, window_width_/2, window_height_/2);
        glfwGetCursorPos(window, &last_x_, &last_y_);
        
        first_call_ = false;
    } else {
        double x_shift = (double)(last_x_ - x) / window_width_;
        double y_shift = (double)(last_y_ - y) / window_height_;
        
        controls.moveAround(x_shift, y_shift);
        
        last_x_ = x;
        last_y_ = y;
    }
}

void mouseWheel(GLFWwindow* window, double x, double y) {
    GLfloat minHeightTerrain;
    controls.moveVertically(y, minHeightTerrain);
}

void mouseButton(GLFWwindow* window, int button, int action, int mod) {}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if(action == GLFW_PRESS){
        switch (key) {
        case (GLFW_KEY_ESCAPE):
            glfwSetWindowShouldClose(window, GL_TRUE);
            break;
        case (GLFW_KEY_P):
            cam_demo_enabled = !cam_demo_enabled;
            if(cam_demo_enabled) {
                camera_path = CameraBezierPath(cam_, controls);
                controls.fps_off();
            }
            break;
        case (GLFW_KEY_UP):
            transformations_.tweak_tess_factor(1);
            break;
        case (GLFW_KEY_DOWN):
            transformations_.tweak_tess_factor(-1);
            break;

        case (GLFW_KEY_T):
            transformations_.toggle_tess_demo();
            break;
        case (GLFW_KEY_N):
            transformations_.toggle_normal_mode();
            break;
        case (GLFW_KEY_R):
            transformations_.toggle_raymarching();
            break;
        case (GLFW_KEY_F):
            controls.toggle_fps();
            break;
        }
    }
}

int main(int argc, char *argv[]) {
    initGlfw();
    glfwSetErrorCallback(errorCallback);
    initWindow();

    window_ = openWindow(window_title_);
    glfwMakeContextCurrent(window_);
    glfwSetKeyCallback(window_, keyCallback);

    // set the mouse press and position callback
    glfwSetMouseButtonCallback(window_, mouseButton);
    glfwSetCursorPosCallback(window_, mousePos);
    glfwSetScrollCallback(window_, mouseWheel);

    glfwSetFramebufferSizeCallback(window_, resizeCallback);
    initGlew();
    std::cout << "OpenGL" << glGetString(GL_VERSION) << std::endl;
    init(window_);
    keyCallback(window_, GLFW_KEY_KP_1, 0, 0, 0);

    glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);




    /// Render loop & keyboard input
    while(!glfwWindowShouldClose(window_)){
        display();
        glfwSwapBuffers(window_);
        glfwPollEvents();
    }

    sky_.cleanup();
    cube_.cleanup();
    terrain_.cleanup();
    noise_.cleanup();
    water_.cleanup();
    closeWindow(window_);
}
