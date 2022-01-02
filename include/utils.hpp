#pragma once

#include <GL/glew.h>

namespace utils {
// these are the only objects I'm interested in
enum class gl_object_kind {
  texture,
  buffer,
  framebuffer,
  renderbuffer,
  vertex_array
};

// helper class to avoid some boilerplate in other classes
template <gl_object_kind kind>
class gl_object {
 protected:
  static void deleter(GLenum id) {
    switch (kind) {
      case gl_object_kind::texture:
        glDeleteTextures(1, &id);
        break;
      case gl_object_kind::buffer:
        glDeleteBuffers(1, &id);
        break;
      case gl_object_kind::framebuffer:
        glDeleteFramebuffers(1, &id);
        break;
      case gl_object_kind::vertex_array:
        glDeleteVertexArrays(1, &id);
        break;
      case gl_object_kind::renderbuffer:
        glDeleteRenderbuffers(1, &id);
        break;
    }
  }

  GLuint id_;

 public:
  gl_object() : id_(0){};
  gl_object(gl_object &) = delete;
  gl_object(gl_object &&other) {
    deleter(id_);
    id_ = other.id_;
    other.id_ = 0;
  }
  ~gl_object() { deleter(id_); }
};

using va_base = gl_object<gl_object_kind::vertex_array>;
using fb_base = gl_object<gl_object_kind::framebuffer>;
using rb_base = gl_object<gl_object_kind::renderbuffer>;
using bf_base = gl_object<gl_object_kind::buffer>;
using tx_base = gl_object<gl_object_kind::texture>;
}  // namespace utils
