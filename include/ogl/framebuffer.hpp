#pragma once

#include <GL/glew.h>

#include <iostream>
#include <ogl/texture.hpp>
#include <utils.hpp>
#include <vector>

namespace ogl {

class renderbuffer : utils::rb_base {
  friend class framebuffer;

  renderbuffer(int width, int height) {
    glGenRenderbuffers(1, &id_);
    glBindRenderbuffer(GL_RENDERBUFFER, id_);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
  }

  void bind() const { glBindRenderbuffer(GL_RENDERBUFFER, id_); }
  void unbind() const { glBindRenderbuffer(GL_RENDERBUFFER, 0); }
};

class framebuffer : utils::fb_base {
  renderbuffer depth_buffer_;
  texture2D color_buffer_;

 public:
  framebuffer(int width, int height)
      : depth_buffer_(width, height), color_buffer_(width, height) {
    glCreateFramebuffers(1, &id_);

    glNamedFramebufferTexture(id_, GL_COLOR_ATTACHMENT0, color_buffer_.id_, 0);
    glNamedFramebufferRenderbuffer(id_, GL_DEPTH_STENCIL_ATTACHMENT,
                                   GL_RENDERBUFFER, depth_buffer_.id_);
    if (glCheckNamedFramebufferStatus(id_, GL_FRAMEBUFFER) !=
        GL_FRAMEBUFFER_COMPLETE) {
      std::cout << "aaa";
    }
  }

  void bind_texture(int s) { color_buffer_.bind(s); }

  void bind() const { glBindFramebuffer(GL_FRAMEBUFFER, id_); }

  void unbind() const { glBindFramebuffer(GL_FRAMEBUFFER, 0); }
};
}  // namespace ogl
