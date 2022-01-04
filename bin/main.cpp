#include <GL/glew.h>

#include <SFML/Window.hpp>
#include <core/proxy.hpp>
#include <core/renderer.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <util/arcball.hpp>

ogl::texture3D load_data() {
  namespace fs = std::filesystem;

  auto size = fs::file_size("bin/bonsai.raw");
  std::ifstream file("bin/bonsai.raw",
                     std::ios_base::in | std::ios_base::binary);

  std::vector<char> bytes(size, 0);
  file.read(&bytes[0], size);

  return ogl::texture3D(256, 256, 256, bytes.data());
}

void message_callback(GLenum source, GLenum type, GLuint id, GLenum severity,
                      GLsizei length, GLchar const* message,
                      void const* user_param);

int main() {
  sf::ContextSettings settings;
  settings.majorVersion = 3;
  settings.minorVersion = 3;
  settings.attributeFlags = sf::ContextSettings::Attribute::Core;
  sf::Window win(sf::VideoMode(800, 600), "Volume", sf::Style::Default,
                 settings);
  win.setActive();
  glewInit();
  glDebugMessageCallback(message_callback, nullptr);
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_DEBUG_OUTPUT);

  core::renderer renderer(800, 600);
  core::proxy_geometry proxy;
  utils::arcball arc(800, 600);
  auto vol = load_data();

  bool running = true;
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
        arc.click(math::vec2(event.mouseButton.x, event.mouseButton.y));
      } else if (event.type == sf::Event::MouseButtonReleased) {
        arc.finish();
      } else if (event.type == sf::Event::MouseMoved && arc.is_on_) {
        auto rotation =
            (arc.drag(math::vec2(event.mouseMove.x, event.mouseMove.y)));
        math::mat4x4 scale = mathter::Scale(0.5f, 0.5f, 0.5f);
        math::mat4x4 rot = math::mat4x4(rotation);
        renderer.model() = scale * rot;
      }
    }

    // draw...
    renderer.render(proxy, vol);
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
