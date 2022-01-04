#pragma once

#include <ogl/buffers.hpp>

namespace core {
class proxy_geometry {
  ogl::vertex_buffer<float> vbo_;
  ogl::vertex_array vao_;

 public:
  proxy_geometry();
  proxy_geometry(proxy_geometry&&) = default;

  void prepare_rendering() const { vao_.bind(); }
};
}  // namespace core
