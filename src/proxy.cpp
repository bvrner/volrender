#include <core/proxy.hpp>

namespace {
// the vertices of a two unit cube
constexpr float vertices[] = {
    // Back face
    -1.0f, -1.0f, -1.0f,  // Bottom-left
    1.0f, 1.0f, -1.0f,    // top-right
    1.0f, -1.0f, -1.0f,   // bottom-right
    1.0f, 1.0f, -1.0f,    // top-right
    -1.0f, -1.0f, -1.0f,  // bottom-left
    -1.0f, 1.0f, -1.0f,   // top-left
    // Front face
    -1.0f, -1.0f, 1.0f,  // bottom-left
    1.0f, -1.0f, 1.0f,   // bottom-right
    1.0f, 1.0f, 1.0f,    // top-right
    1.0f, 1.0f, 1.0f,    // top-right
    -1.0f, 1.0f, 1.0f,   // top-left
    -1.0f, -1.0f, 1.0f,  // bottom-left
    // Left face
    -1.0f, 1.0f, 1.0f,    // top-right
    -1.0f, 1.0f, -1.0f,   // top-left
    -1.0f, -1.0f, -1.0f,  // bottom-left
    -1.0f, -1.0f, -1.0f,  // bottom-left
    -1.0f, -1.0f, 1.0f,   // bottom-right
    -1.0f, 1.0f, 1.0f,    // top-right
                          // Right face
    1.0f, 1.0f, 1.0f,     // top-left
    1.0f, -1.0f, -1.0f,   // bottom-right
    1.0f, 1.0f, -1.0f,    // top-right
    1.0f, -1.0f, -1.0f,   // bottom-right
    1.0f, 1.0f, 1.0f,     // top-left
    1.0f, -1.0f, 1.0f,    // bottom-left
    // Bottom face
    -1.0f, -1.0f, -1.0f,  // top-right
    1.0f, -1.0f, -1.0f,   // top-left
    1.0f, -1.0f, 1.0f,    // bottom-left
    1.0f, -1.0f, 1.0f,    // bottom-left
    -1.0f, -1.0f, 1.0f,   // bottom-right
    -1.0f, -1.0f, -1.0f,  // top-right
    // Top face
    -1.0f, 1.0f, -1.0f,  // top-left
    1.0f, 1.0f, 1.0f,    // bottom-right
    1.0f, 1.0f, -1.0f,   // top-right
    1.0f, 1.0f, 1.0f,    // bottom-right
    -1.0f, 1.0f, -1.0f,  // top-left
    -1.0f, 1.0f, 1.0f    // bottom-left
};
}  // namespace

core::proxy_geometry::proxy_geometry()
    : vbo_(vertices, sizeof(vertices) / sizeof(float)), vao_() {
  ogl::buffer_layout::element elem;
  elem.count = 3;
  elem.type = GL_FLOAT;
  elem.size = sizeof(float);
  vao_.bind();
  vbo_.bind();
  vao_.set_elem(elem, 0, sizeof(float) * 3);
  vbo_.unbind();
  vao_.unbind();
}
