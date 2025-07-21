#pragma once

#include <type_traits>
#include <functional>

#include "interpolators.hh"

namespace anim {

template <typename T>
T lerp(T start, T end, float x) = delete;

template <typename T> requires std::is_arithmetic_v<T>
[[nodiscard]] inline constexpr T lerp(T start, T end, float x) {
    return start + x * (end - start);
}



#ifdef ANIM_INTEGRATION_RAYLIB

template <>
[[nodiscard]] inline constexpr Vector2 lerp(Vector2 start, Vector2 end, float x) {
    return Vector2Lerp(start, end, x);
}

template <>
[[nodiscard]] inline constexpr Color lerp(Color start, Color end, float x) {
    return ColorLerp(start, end, x);
}

template <>
[[nodiscard]] inline constexpr Rectangle lerp(Rectangle start, Rectangle end, float x) {
    return {
        anim::lerp(start.x, end.x, x),
        anim::lerp(start.y, end.y, x),
        anim::lerp(start.width, end.width, x),
        anim::lerp(start.height, end.height, x)
    };
}

#endif // ANIM_INTEGRATION_RAYLIB



// concept for a type that may be interpolated
// custom types may be added via template specialization of anim::lerp()

template <typename T>
concept Interpolatable = requires (T start, T end, float x) {
    lerp(start, end, x);
};

// a transition between two values
template <Interpolatable T>
class Interpolator {
    using InterpFn = std::function<float(float)>;

    const T m_start;
    const T m_end;
    const double m_duration;
    const InterpFn m_fn;

public:
    Interpolator() : Interpolator(1.0f) { }
    explicit Interpolator(T end) : Interpolator(0.0f, end) { }
    Interpolator(T start, T end) : Interpolator(start, end, 1.0f) { }

    Interpolator(T start, T end, double duration)
    : Interpolator(start, end, duration, [](float x) { return x; })
    { }

    Interpolator(T start, T end, double duration, InterpFn fn)
        : m_start(start)
        , m_end(end)
        , m_duration(duration)
        , m_fn(fn)
    { }

    // create an interpolator that maintains the specified value for the given duration
    [[nodiscard]] static constexpr Interpolator<T> wait(T value, double duration) {
        return Interpolator(value, value, duration, interpolators::step);
    }

    [[nodiscard]] T get_start() const {
        return m_start;
    }

    [[nodiscard]] T get_end() const {
        return m_end;
    }

    [[nodiscard]] double get_duration() const {
        return m_duration;
    }

    operator T() const {
        return get();
    }

    [[nodiscard]] T get(double t) const {
        double x = t / m_duration;
        return anim::lerp(m_start, m_end, m_fn(x));
    }

};

// TODO: pause/resume semantics?
struct IAnimation {
    virtual void start() = 0;
    virtual void reset() = 0;
    [[nodiscard]] virtual double get_progress() const = 0; // 0..1
    [[nodiscard]] virtual double get_duration() const = 0;
    [[nodiscard]] virtual bool is_stopped() const = 0;
    [[nodiscard]] virtual bool is_done() const = 0;
    [[nodiscard]] virtual bool is_running() const = 0;
    virtual ~IAnimation() = default;
};

}
