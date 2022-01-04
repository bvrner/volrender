#pragma once

#include <Mathter/Matrix.hpp>
#include <Mathter/Quaternion.hpp>
#include <Mathter/Vector.hpp>

namespace math {
template <bool Packed = false>
using quat = mathter::Quaternion<float, Packed>;
using mat3x3 =
    mathter::Matrix<float, 3, 3, mathter::eMatrixOrder::PRECEDE_VECTOR,
                    mathter::eMatrixLayout::COLUMN_MAJOR, true>;
using mat4x4 =
    mathter::Matrix<float, 4, 4, mathter::eMatrixOrder::PRECEDE_VECTOR,
                    mathter::eMatrixLayout::COLUMN_MAJOR, true>;
using vec2 = mathter::Vector<float, 2, true>;
using vec3 = mathter::Vector<float, 3, true>;
}  // namespace math
