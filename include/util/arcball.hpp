#include <cmath>
#include <util/matdef.hpp>

namespace utils {
class arcball {
  int width_;
  int height_;
  math::vec2 current_;
  math::vec3 click_vec_;
  math::vec3 drag_vec_;
  math::quat this_rot_;
  math::quat last_rot_;

 public:
  bool is_on_;

  arcball(int width, int height)
      : width_(width),
        height_(height),
        current_(0.0, 0.0),
        click_vec_(0.0, 0.0, 0.0),
        drag_vec_(0.0, 0.0, 0.0),
        this_rot_(1.0, 0.0, 0.0, 0.0),
        last_rot_(1.0, 0.0, 0.0, 0.0) {}

  void update(int width, int height) {
    width_ = width;
    height_ = height;
  }

  void click(math::vec2 point) {
    is_on_ = true;
    current_ = point;
    click_vec_ = current_vec();
  }

  math::vec3 current_vec() {
    auto temp = current_;

    temp.x = 1.0 * temp.x / width_ * 2.0 - 1.0;
    temp.y = -(1.0 * temp.y / height_ * 2.0 - 1.0);

    auto len = (temp.x * temp.x) + (temp.y * temp.y);

    if (len <= 1.0)
      return math::vec3(temp.x, temp.y, std::sqrt(1.0 * 1.0 - len));
    else
      return mathter::Normalize(math::vec3(temp.x, temp.y, 0.0));
  }

  math::quat drag(math::vec2 point) {
    current_ = point;
    drag_vec_ = current_vec();

    math::vec3 perp = mathter::Cross(click_vec_, drag_vec_);

    this_rot_ = math::quat(mathter::Dot(click_vec_, drag_vec_), perp);
    return this_rot_ * last_rot_;
  }

  void finish() {
    is_on_ = false;
    last_rot_ = this_rot_ * last_rot_;
  }
};
}  // namespace utils
