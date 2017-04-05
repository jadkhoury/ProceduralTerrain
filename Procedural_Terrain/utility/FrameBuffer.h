#pragma once
#include "icg_helper.h"

class FrameBuffer {

private:
    int width_;
    int height_;
    GLuint framebuffer_object_id_;
    GLuint depth_render_buffer_id_;
    GLuint color_texture_id_, depthTexture_;

public:
    // warning: overrides viewport!!
    void bind() {
        glViewport(0, 0, width_, height_);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_object_id_);
        const GLenum buffers[] = { GL_COLOR_ATTACHMENT0 };
        glDrawBuffers(1 /*length of buffers[]*/, buffers);
    }

    void unBind() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    int init(int image_width, int image_height, int datatype, bool use_interpolation = false) {
        width_ = image_width;
        height_ = image_height;

        if(datatype == GL_DEPTH_COMPONENT16){
            glGenFramebuffers(1, &framebuffer_object_id_);
            glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_object_id_);

            // Depth texture. Slower than a depth buffer, but you can sample it later in your shader
            glGenTextures(1, &depthTexture_);
            glBindTexture(GL_TEXTURE_2D, depthTexture_);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, width_, height_, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

            glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture_, 0);

            // No color output in the bound framebuffer, only depth.
            glDrawBuffer(GL_NONE);

            return depthTexture_;


        } else {


            // create color attachment
            {
                glGenTextures(1, &color_texture_id_);
                glBindTexture(GL_TEXTURE_2D, color_texture_id_);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

                if(use_interpolation){
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                } else {
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                }

                // create texture for the color attachment
                // see Table.2 on
                // khronos.org/opengles/sdk/docs/man3/docbook4/xhtml/glTexImage2D.xml
                glTexImage2D(GL_TEXTURE_2D, 0, datatype, width_, height_, 0,
                             (datatype == GL_RGB8 ? GL_RGB : GL_RED), GL_UNSIGNED_BYTE, NULL);
            }

            // create render buffer (for depth channel)
            {
                glGenRenderbuffers(1, &depth_render_buffer_id_);
                glBindRenderbuffer(GL_RENDERBUFFER, depth_render_buffer_id_);
                glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, width_, height_);
                glBindRenderbuffer(GL_RENDERBUFFER, 0);
            }

            // tie it all together
            {
                glGenFramebuffers(1, &framebuffer_object_id_);
                glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_object_id_);
                glFramebufferTexture2D(GL_FRAMEBUFFER,
                                       GL_COLOR_ATTACHMENT0 /*location = 0*/,
                                       GL_TEXTURE_2D, color_texture_id_,
                                       0 /*level*/);
                glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                                          GL_RENDERBUFFER, depth_render_buffer_id_);

                if (glCheckFramebufferStatus(GL_FRAMEBUFFER) !=
                        GL_FRAMEBUFFER_COMPLETE) {
                    cerr << "!!!ERROR: Framebuffer not OK :(" << endl;
                }
                glBindFramebuffer(GL_FRAMEBUFFER, 0); // avoid pollution
            }

            return color_texture_id_;
        }
    }

    void cleanup() {
        glDeleteTextures(1, &color_texture_id_);
        glDeleteRenderbuffers(1, &depth_render_buffer_id_);
        glBindFramebuffer(GL_FRAMEBUFFER, 0 /*UNBIND*/);
        glDeleteFramebuffers(1, &framebuffer_object_id_);
    }
};
