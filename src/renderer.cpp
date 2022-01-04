#include <core/renderer.hpp>

// shader sources, no need to load them dynamically from files
namespace {
constexpr char general_vs[] =
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

constexpr char faces_fs[] =
    R"(#version 330 core
out vec4 Col;
in vec4 Position;
void main()
{
Col = Position;
})";

constexpr char raycast_fs[] =
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
}  // namespace

core::renderer::renderer(int width, int height)
    : front_faces_(width, height),
      back_faces_(width, height),
      matrices_buff_(nullptr, 3) {
  // setup de shaders
  auto general_vs_comp = ogl::vertex_shader(general_vs);
  auto faces_fs_comp = ogl::frag_shader(faces_fs);
  auto raycast_fs_comp = ogl::frag_shader(raycast_fs);

  faces_shader_ = ogl::shader_program(general_vs_comp, faces_fs_comp);
  raycast_shader_ = ogl::shader_program(general_vs_comp, raycast_fs_comp);
  faces_shader_.uniform_block_binding("Matrix", 0);
  raycast_shader_.uniform_block_binding("Matrix", 0);

  raycast_shader_.set_sampler("front", 0);
  raycast_shader_.set_sampler("back", 1);
  raycast_shader_.set_sampler("volume", 2);

  matrices_buff_.bind_base(0);

  // initiliaze transformations to sane values
  transforms_[0] = mathter::Scale(0.5f, 0.5f, 0.5f);
  transforms_[1] =
      mathter::LookAt(math::vec3(0.0, 0.0, 3.0), math::vec3(0.0, 0.0, 0.0),
                      math::vec3(0.0, 1.0, 0.0), true, false, false);
  transforms_[2] = mathter::Perspective(
      mathter::Deg2Rad(45.0f), 800.0f / 600.0f, 0.1f, 100.0f, 0.0f, 1.0f);

  // upload data to uniform buffer
  matrices_buff_.buffer_data(transforms_.data(), 3);
}

void core::renderer::render(const proxy_geometry& geometry,
                            const ogl::texture3D& volume) {
  matrices_buff_.buffer_data(transforms_.data(), 3);
  geometry.prepare_rendering();
  faces_shader_.bind();
  // draw front faces
  front_faces_.bind();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glCullFace(GL_BACK);
  glDrawArrays(GL_TRIANGLES, 0, 36);
  front_faces_.unbind();

  // draw back faces
  back_faces_.bind();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glCullFace(GL_FRONT);
  glDrawArrays(GL_TRIANGLES, 0, 36);
  back_faces_.unbind();

  faces_shader_.unbind();

  // raycast
  raycast_shader_.bind();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  front_faces_.bind_texture(0);
  back_faces_.bind_texture(1);
  volume.bind(2);
  glDrawArrays(GL_TRIANGLES, 0, 36);
  raycast_shader_.unbind();
}
