#pragma once

#include <cmath>

namespace anim {

namespace interpolators {

#define ANIM_IMPL_INTERP_FN(ident) \
    [[nodiscard]] inline constexpr float ident(float x) noexcept

ANIM_IMPL_INTERP_FN (step) {
    (void) x;
    return 1.0f;
}

ANIM_IMPL_INTERP_FN (linear) {
    return x;
}

ANIM_IMPL_INTERP_FN (ease_in_quad) {
    return std::pow(x, 2);
}

ANIM_IMPL_INTERP_FN (ease_in_out_quad) {
    return x < 0.5 ? 2 * x * x : 1 - std::pow(-2 * x + 2, 2) / 2;
}

ANIM_IMPL_INTERP_FN (ease_in_cubic) {
    return std::pow(x, 3);
}

ANIM_IMPL_INTERP_FN (ease_out_expo) {
    return x == 1 ? 1 : 1 - std::pow(2, -10 * x);
}

ANIM_IMPL_INTERP_FN (ease_in_out_cubic) {
    return x < 0.5 ? 4 * std::pow(x, 3) : 1 - std::pow(-2 * x + 2, 3) / 2;
}

ANIM_IMPL_INTERP_FN (ease_in_out_back) {
    float c1 = 1.70158;
    float c2 = c1 * 1.525;

    return x < 0.5
    ? (std::pow(2 * x, 2) * ((c2 + 1) * 2 * x - c2)) / 2
    : (std::pow(2 * x - 2, 2) * ((c2 + 1) * (x * 2 - 2) + c2) + 2) / 2;
}

ANIM_IMPL_INTERP_FN (ease_in_out_circ) {
    return x < 0.5
    ? (1 - std::sqrt(1 - std::pow(2 * x, 2))) / 2
    : (std::sqrt(1 - std::pow(-2 * x + 2, 2)) + 1) / 2;
}

ANIM_IMPL_INTERP_FN (ease_in_out_quint) {
    return x < 0.5 ? 16 * std::pow(x, 5) : 1 - std::pow(-2 * x + 2, 5) / 2;
}

ANIM_IMPL_INTERP_FN (ease_out_elastic) {
    float c4 = (2 * M_PI) / 3;
    return x == 0 ? 0 : x == 1 ? 1 : std::pow(2, -10 * x) * std::sin((x * 10 - 0.75) * c4) + 1;
}

ANIM_IMPL_INTERP_FN (ease_in_expo) {
    return x == 0 ? 0 : std::pow(2, 10 * x - 10);
}

ANIM_IMPL_INTERP_FN (ease_out_back) {
    float c1 = 1.70158;
    float c3 = c1 + 1;
    return 1 + c3 * std::pow(x - 1, 3) + c1 * std::pow(x - 1, 2);
}

ANIM_IMPL_INTERP_FN (ease_in_out_expo) {
    return x == 0 ? 0 : x == 1 ? 1
    : x < 0.5 ? std::pow(2, 20 * x - 10) / 2
    : (2 - std::pow(2, -20 * x + 10)) / 2;
}

}

}
