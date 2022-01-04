#pragma once

#include <array>
#include <matdef.hpp>
#include <ogl/framebuffer.hpp>
#include <ogl/shader.hpp>
#include <ogl/texture.hpp>
#include <proxy.hpp>

namespace core {
class renderer {
  ogl::framebuffer front_faces_;
  ogl::framebuffer back_faces_;
  ogl::shader_program faces_shader_;
  ogl::shader_program raycast_shader_;
  ogl::uniform_buffer<math::mat4x4> matrices_buff_;

  // model - view - projection, in order
  std::array<math::mat4x4, 3> transforms_;

 public:
  renderer() = delete;
  renderer(renderer&) = delete;
  renderer(renderer&&) = default;
  renderer(int width, int height);

  void render(const proxy_geometry&, const ogl::texture3D&);
  math::mat4x4& model() { return transforms_[0]; }
  math::mat4x4& view() { return transforms_[1]; }
  math::mat4x4& projection() { return transforms_[2]; }
};
}  // namespace core
