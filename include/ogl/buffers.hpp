#pragma once

#include <GL/glew.h>

#include <utils.hpp>
#include <vector>

namespace ogl {
enum class buffer_type {
  vertex = GL_ARRAY_BUFFER,
  element = GL_ELEMENT_ARRAY_BUFFER,
  uniform = GL_UNIFORM_BUFFER
};

template <typename T, buffer_type type>
struct gl_buffer : utils::bf_base {
  gl_buffer(const T *data, size_t len) {
    GLuint id = 0;
    glGenBuffers(1, &id);
    glBindBuffer(static_cast<GLenum>(type), id);
    auto size = len * sizeof(T);
    glBufferStorage(static_cast<GLenum>(type), size, data,
                    GL_DYNAMIC_STORAGE_BIT);
    glBindBuffer(static_cast<GLenum>(type), 0);

    id_ = id;
  }

  void buffer_data(const T *data, size_t len) {
    bind();
    glBufferSubData(static_cast<GLenum>(type), 0, sizeof(T) * len, data);
    unbind();
  }
  void bind() const { glBindBuffer(static_cast<GLenum>(type), id_); }
  void unbind() const { glBindBuffer(static_cast<GLenum>(type), 0); }
};

template <typename T>
struct uniform_buffer : gl_buffer<T, buffer_type::uniform> {
  using gl_buffer<T, buffer_type::uniform>::gl_buffer;
  void bind_base(GLuint index) const {
    glBindBufferBase(GL_UNIFORM_BUFFER, index, this->id_);
  }
};
template <typename T>
using vertex_buffer = gl_buffer<T, buffer_type::vertex>;
template <typename T>
using element_buffer = gl_buffer<T, buffer_type::element>;

struct buffer_layout {
  struct element {
    int count;
    GLenum type;
    int size;
  };

  std::vector<element> elements;
  GLsizei stride;

  void push(int count, GLenum type, int size) {
    element elem;
    elem.count = count;
    elem.type = type;
    elem.size = size;

    stride += size * count;
    elements.push_back(elem);
  }
};

class vertex_array : utils::va_base {
 public:
  vertex_array() : utils::va_base() {
    GLuint id;
    glGenVertexArrays(1, &id);
    id_ = id;
  }
  // configures this VAO with a determined buffer and layout
  template <typename T>
  void config_buffer(const vertex_buffer<T> &buffer,
                     const buffer_layout &layout) {
    bind();
    buffer.bind();
    int offset = 0;
    int index = 0;
    for (auto &elem : layout.elements) {
      set_elem(elem, index, layout.stride, offset * elem.size);
      offset += elem.count;
      index++;
    }
    buffer.unbind();
    unbind();
  }

  // sets a single buffer element into this VAO, assumes a buffer and this are
  // already bound
  void set_elem(buffer_layout::element elem, int index, GLsizei stride = 0,
                GLintptr offset = 0) {
    glEnableVertexAttribArray(index);
    glVertexAttribPointer(index, elem.count, elem.type, GL_FALSE, stride,
                          (GLvoid *)offset);
  }
  void bind() const { glBindVertexArray(id_); }

  void unbind() const { glBindVertexArray(0); }
};

}  // namespace ogl
