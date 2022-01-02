#pragma once

#include <GL/glew.h>

#include <iostream>
#include <utils.hpp>
namespace ogl {
enum class texture_type { tex2d = GL_TEXTURE_2D, tex3d = GL_TEXTURE_3D };

template <texture_type type>
struct texture : utils::tx_base {
  friend class framebuffer;
  void create() {
    deleter(id_);
    glCreateTextures(static_cast<GLenum>(type), 1, &id_);
  }
  void texture_param(GLenum pname, GLint param) const {
    glTextureParameteri(id_, pname, param);
  }

  void bind(int slot) {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(static_cast<GLenum>(type), id_);
  }
};

struct texture2D : texture<texture_type::tex2d> {
  texture2D(int width, int height, void *pixels = nullptr) {
    create();

    texture_param(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    texture_param(GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTextureStorage2D(id_, 1, GL_RGB8, width, height);
    if (pixels != nullptr)
      glTextureSubImage2D(id_, 0, 0, 0, width, height, GL_RGBA,
                          GL_UNSIGNED_BYTE, pixels);
  }
};

struct texture3D : texture<texture_type::tex3d> {
  texture3D(int width, int height, int depth, void *pixels = nullptr) {
    GLuint name;
    glCreateTextures(GL_TEXTURE_3D, 1, &name);

    // texture_param(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    // texture_param(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // texture_param(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    // texture_param(GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTextureStorage3D(name, 1, GL_R8, width, height, depth);
    if (pixels != nullptr)
      glTextureSubImage3D(name, 0, 0, 0, 0, width, height, depth, GL_RED,
                          GL_UNSIGNED_BYTE, pixels);
    id_ = name;
  }
};
}  // namespace ogl
