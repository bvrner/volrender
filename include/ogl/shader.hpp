#pragma once

#include <GL/glew.h>

#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>

namespace ogl {
enum class shader_type {
  vertex = GL_VERTEX_SHADER,
  fragment = GL_FRAGMENT_SHADER
};

template <shader_type type>
class shader {
  friend class shader_program;
  GLuint id_;

 public:
  shader() = delete;
  shader(shader &) = delete;
  shader(shader &&other) : id_(other.id_) { other.id_ = 0; }

  shader(std::string_view src) {
    auto id = glCreateShader(static_cast<GLenum>(type));

    const auto data = src.data();
    const auto size = static_cast<GLint>(src.size());
    glShaderSource(id, 1, &data, &size);
    glCompileShader(id);

    GLint success = 0;
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);

    if (success != GL_TRUE) {
      GLint log_len = 0;
      glGetShaderiv(id, GL_INFO_LOG_LENGTH, &log_len);
      std::string info_log(log_len, '\0');
      glGetShaderInfoLog(id, log_len, nullptr, info_log.data());

      glDeleteShader(id);
      throw std::runtime_error(info_log);
    }

    id_ = id;
  }
  ~shader() { glDeleteShader(id_); }
};

using vertex_shader = shader<shader_type::vertex>;
using frag_shader = shader<shader_type::fragment>;

class shader_program {
  GLuint id_;

  static void check_program_status(GLuint prog, GLenum which) {
    GLint status = GL_TRUE;

    glGetProgramiv(prog, which, &status);

    if (status != GL_TRUE) {
      GLint length = 0;

      glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &length);
      std::string info_log(length, '\0');

      glGetProgramInfoLog(prog, length, nullptr, info_log.data());

      glDeleteProgram(prog);
      throw std::runtime_error(info_log);
    }
  }

 public:
  shader_program() = delete;
  shader_program(shader_program &) = delete;
  shader_program(shader_program &&other) : id_(other.id_) { other.id_ = 0; }

  shader_program(const shader<shader_type::vertex> &v_shader,
                 const shader<shader_type::fragment> &f_shader) {
    id_ = glCreateProgram();
    glAttachShader(id_, v_shader.id_);
    glAttachShader(id_, f_shader.id_);

    glLinkProgram(id_);
    check_program_status(id_, GL_LINK_STATUS);
    set_sampler("volume", 2);
    set_sampler("front", 0);
    set_sampler("back", 1);
    glValidateProgram(id_);
    check_program_status(id_, GL_VALIDATE_STATUS);

    glDetachShader(id_, v_shader.id_);
    glDetachShader(id_, f_shader.id_);
  }
  void uniform_block_binding(const std::string_view name,
                             GLuint binding) const {
    auto index = glGetUniformBlockIndex(id_, name.data());
    glUniformBlockBinding(id_, index, binding);
  }

  void set_sampler(std::string_view name, int val) {
    bind();
    glUniform1i(glGetUniformLocation(id_, name.data()), val);
    unbind();
  }

  void bind() const { glUseProgram(id_); }
  void unbind() const { glUseProgram(0); }
  ~shader_program() { glDeleteProgram(id_); }
};

}  // namespace ogl
