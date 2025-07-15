#pragma once

#include <print>
#include <cassert>
#include <algorithm>
#include <functional>
#include <chrono>
#include <cmath>
#include <numeric>

#include "interpolators.hh"
#include "batch.hh"
#include "sequence.hh"
#include "animation.hh"

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

    const InterpFn m_fn;
    const T m_start;
    const T m_end;
    const double m_duration;

public:
    [[nodiscard]] T get_start() const {
        return m_start;
    }

    [[nodiscard]] T get_end() const {
        return m_end;
    }

    [[nodiscard]] double get_duration() const {
        return m_duration;
    }

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

    operator T() const {
        return get();
    }

    [[nodiscard]] T get(double t) const {
        double x = t / m_duration;
        return anim::lerp(m_start, m_end, m_fn(x));
    }

};

// runs a list of interpolators synchronously
template <Interpolatable T>
class Animation : public IAnimation {
    std::vector<Interpolator<T>> m_interps;
    double m_start_time = 0.0f;
    bool m_is_active = false;

public:
    Animation() = default;

    Animation(std::initializer_list<Interpolator<T>> interps)
    : m_interps(interps)
    { }

    Animation(Interpolator<T> interp) : m_interps { interp } { }

    void add(Interpolator<T> interp) {
        m_interps.push_back(interp);
    }

    void start() override {
        m_is_active = true;
        m_start_time = get_time_secs();
    }

    void reset() override {
        m_is_active = false;
        m_start_time = 0.0f;
    }

    [[nodiscard]] double get_duration() const override {

        auto fn = [](double acc, anim::Interpolator<T> const& interp) {
            return acc + interp.get_duration();
        };

        return std::accumulate(m_interps.cbegin(), m_interps.cend(), 0.0f, fn);
    }

    [[nodiscard]] bool is_stopped() const override {
        return !m_is_active;
    }

    [[nodiscard]] bool is_running() const override {
        if (is_done()) return false;
        return m_is_active;
    }

    [[nodiscard]] bool is_done() const override {
        if (!m_is_active) return false;
        return get_time() >= get_duration();
    }

    [[nodiscard]] T get(float t) const {

        double time_to_interp = 0.0f;

        auto fn = [&](Interpolator<T> const& interp) {
            time_to_interp += interp.get_duration();
            bool is_current = t <= time_to_interp;
            return is_current;
        };

        auto current = std::ranges::find_if(m_interps, fn);
        assert(current != m_interps.end());

        double diff = time_to_interp - t;
        return current->get(current->get_duration() - diff);
    }

    [[nodiscard]] T get() const {
        if (m_is_active) {
            if (is_done()) return m_interps.back().get_end();
            double t = get_time();
            return get(t);

        } else {
            return m_interps.front().get_start();
        }
    }

    T const* operator->() const {
        static T x;
        x = get();
        return &x;
    }

    operator T() const {
        return get();
    }

private:
    [[nodiscard]] double get_time() const {
        return get_time_secs() - m_start_time;
    }

    [[nodiscard]] static inline double get_time_secs() {
        namespace chrono = std::chrono;

        auto now = chrono::steady_clock::now();
        auto time = now.time_since_epoch();
        return chrono::duration_cast<chrono::duration<double>>(time).count();
    }

};







class AnimationTemplate : public IAnimation {
protected:
    Sequence m_anim;
    AnimationTemplate() = default;
    AnimationTemplate(Sequence anim) : m_anim(anim) { }
    virtual void on_update() { }

public:
    void update() {
        m_anim.dispatch();
        on_update();
    }

    void start() override {
        m_anim.start();
    }

    void reset() override {
        m_anim.reset();
    }

    [[nodiscard]] double get_duration() const override {
        return m_anim.get_duration();
    }

    [[nodiscard]] bool is_stopped() const override {
        return m_anim.is_stopped();
    }

    [[nodiscard]] bool is_done() const override {
        return m_anim.is_done();
    }

    [[nodiscard]] bool is_running() const override {
        return m_anim.is_running();
    }

};








}
