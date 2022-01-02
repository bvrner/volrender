#include <GL/glew.h>

#include <Mathter/IoStream.hpp>
#include <Mathter/Matrix.hpp>
#include <SFML/Window.hpp>
#include <arcball.hpp>
#include <buffers.hpp>
#include <filesystem>
#include <framebuffer.hpp>
#include <fstream>
#include <iostream>
#include <ogl/shader.hpp>
#include <ogl/texture.hpp>

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

constexpr char vs[] =
    R"(#version 330 core
layout (location = 0) in vec3 aPos;
layout (std140) uniform Matrix
{
    mat4 model;
    mat4 view;
    mat4 proj;
};
out vec4 Position;
out vec4 tPosition;
void main()
{
  Position = vec4(0.5 * (aPos + 1.0), 1.0);
  tPosition = proj * view * model * vec4(aPos, 1.0);
  gl_Position = tPosition;
})";

constexpr char fs[] =
    R"(#version 330 core
out vec4 Col;
in vec4 Position;
void main()
{
Col = Position;
})";

constexpr char ray_fs[] =
    R"(#version 330 core
out vec4 Col;
in vec4 Position;

uniform sampler2D front;
uniform sampler2D back;
uniform sampler3D volume;

void main()
{
   vec2 texC = gl_FragCoord.st/vec2(800, 600);

   vec3 ray_start = texture(front, texC).xyz;
   vec3 ray_end = texture(back, texC).xyz;
   vec3 ray = ray_end - ray_start;
   float ray_length = length(ray);
   vec3 step = 0.01 * ray / ray_length;

   vec3 position = ray_start;

   float max = 0.0;
   while(ray_length > 0) {
     float intensity = texture(volume, position).r;

     if (intensity > max)
        max = intensity;

     ray_length -= 0.01;
     position += step;

   }

   Col = vec4(vec3(max), 1.0);
})";

rendering::texture3D load_data() {
  namespace fs = std::filesystem;

  auto size = fs::file_size("bin/bonsai.raw");
  std::ifstream file("bin/bonsai.raw",
                     std::ios_base::in | std::ios_base::binary);

  std::vector<char> bytes(size, 0);
  file.read(&bytes[0], size);

  return rendering::texture3D(256, 256, 256, bytes.data());
}

void message_callback(GLenum source, GLenum type, GLuint id, GLenum severity,
                      GLsizei length, GLchar const* message,
                      void const* user_param);
using mat4x4 =
    mathter::Matrix<float, 4, 4, mathter::eMatrixOrder::PRECEDE_VECTOR,
                    mathter::eMatrixLayout::COLUMN_MAJOR, true>;
using vec3 = mathter::Vector<float, 3, true>;

int main() {
  sf::ContextSettings settings;
  settings.majorVersion = 3;
  settings.minorVersion = 3;
  settings.attributeFlags = sf::ContextSettings::Attribute::Core;
  sf::Window win(sf::VideoMode(800, 600), "Test", sf::Style::Default, settings);
  win.setActive();
  glewInit();

  rendering::vertex_shader gv(vs);
  rendering::frag_shader ffs(fs);
  rendering::frag_shader rfs(ray_fs);
  rendering::shader_program first_pass = rendering::shader_program(gv, ffs);
  rendering::shader_program second_pass = rendering::shader_program(gv, rfs);
  rendering::framebuffer front(800, 600);
  rendering::framebuffer back(800, 600);
  first_pass.uniform_block_binding("Matrix", 0);
  second_pass.uniform_block_binding("Matrix", 0);
  second_pass.set_sampler("front", 0);
  second_pass.set_sampler("back", 1);
  second_pass.set_sampler("volume", 2);
  mat4x4 mats[3] = {};
  mats[0] = mathter::Scale(0.5f, 0.5f, 0.5f);
  mats[1] = mathter::LookAt(vec3(0.0, 0.0, 3.0), vec3(0.0, 0.0, 0.0),
                            vec3(0.0, 1.0, 0.0), true, false, false);
  mats[2] = mathter::Perspective(mathter::Deg2Rad(45.0f), 800.0f / 600.0f, 0.1f,
                                 100.0f, 0.0f, 1.0f);
  rendering::vertex_array va;
  rendering::vertex_buffer buf(vertices, sizeof(vertices) / sizeof(float));
  rendering::uniform_buffer ubuf(mats, 3);
  rendering::buffer_layout::element elem;
  elem.count = 3;
  elem.type = GL_FLOAT;
  elem.size = sizeof(float);
  va.bind();
  buf.bind();
  va.set_elem(elem, 0, sizeof(float) * 3);
  buf.unbind();
  va.unbind();
  bool running = true;

  ubuf.bind_base(0);

  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(message_callback, nullptr);
  utils::arcball arc(800, 600);
  auto vol = load_data();
  while (running) {
    // handle events
    sf::Event event;
    while (win.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        // end the program
        running = false;
      } else if (event.type == sf::Event::Resized) {
        // adjust the viewport when the window is resized
        glViewport(0, 0, event.size.width, event.size.height);
      } else if (event.type == sf::Event::MouseButtonPressed &&
                 event.mouseButton.button == sf::Mouse::Button::Left) {
        arc.click(vec2(event.mouseButton.x, event.mouseButton.y));
      } else if (event.type == sf::Event::MouseButtonReleased) {
        arc.finish();
      } else if (event.type == sf::Event::MouseMoved && arc.is_on_) {
        auto rotation = (arc.drag(vec2(event.mouseMove.x, event.mouseMove.y)));
        mat4x4 scale = mathter::Scale(0.5f, 0.5f, 0.5f);
        mat4x4 rot = mat4x4(rotation);
        mats[0] = scale * rot;

        ubuf.buffer_data(mats, 3);
      }
    }

    // clear the buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // draw...
    va.bind();
    first_pass.bind();
    front.bind();
    glCullFace(GL_BACK);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    front.unbind();

    back.bind();
    glCullFace(GL_FRONT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    back.unbind();
    first_pass.unbind();

    second_pass.bind();
    front.bind_texture(0);
    back.bind_texture(1);
    vol.bind(2);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    va.unbind();
    second_pass.unbind();
    // end the current frame (internally swaps the front and back buffers)
    win.display();
  }
  return 0;
}

void message_callback(GLenum source, GLenum type, GLuint id, GLenum severity,
                      [[maybe_unused]] GLsizei length, GLchar const* message,
                      [[maybe_unused]] void const* user_param) {
  auto const src_str = [source]() {
    switch (source) {
      case GL_DEBUG_SOURCE_API:
        return "API";
      case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        return "WINDOW SYSTEM";
      case GL_DEBUG_SOURCE_SHADER_COMPILER:
        return "SHADER COMPILER";
      case GL_DEBUG_SOURCE_THIRD_PARTY:
        return "THIRD PARTY";
      case GL_DEBUG_SOURCE_APPLICATION:
        return "APPLICATION";
      case GL_DEBUG_SOURCE_OTHER:
        return "OTHER";
      default:
        return "";
    }
  }();

  auto const type_str = [type]() {
    switch (type) {
      case GL_DEBUG_TYPE_ERROR:
        return "ERROR";
      case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        return "DEPRECATED_BEHAVIOR";
      case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        return "UNDEFINED_BEHAVIOR";
      case GL_DEBUG_TYPE_PORTABILITY:
        return "PORTABILITY";
      case GL_DEBUG_TYPE_PERFORMANCE:
        return "PERFORMANCE";
      case GL_DEBUG_TYPE_MARKER:
        return "MARKER";
      case GL_DEBUG_TYPE_OTHER:
        return "OTHER";
      default:
        return "";
    }
  }();

  auto const severity_str = [severity]() {
    switch (severity) {
      case GL_DEBUG_SEVERITY_NOTIFICATION:
        return "NOTIFICATION";
      case GL_DEBUG_SEVERITY_LOW:
        return "LOW";
      case GL_DEBUG_SEVERITY_MEDIUM:
        return "MEDIUM";
      case GL_DEBUG_SEVERITY_HIGH:
        return "HIGH";
      default:
        return "";
    }
  }();
  std::cout << src_str << ", " << type_str << ", " << severity_str << ", " << id
            << ": " << message << '\n';
}
