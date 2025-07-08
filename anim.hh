#pragma once

#include <cassert>
#include <functional>
#include <chrono>
#include <cmath>
#include <numeric>

namespace anim {

namespace interpolators {

[[nodiscard, maybe_unused]] static float step(float) {
    return 1.0f;
}

[[nodiscard, maybe_unused]] static float linear(float x) {
    return x;
}

[[nodiscard, maybe_unused]] static float squared(float x) {
    return std::pow(x, 2);
}

[[nodiscard, maybe_unused]] static float cubed(float x) {
    return std::pow(x, 3);
}

[[nodiscard, maybe_unused]] static float ease_out_expo(float x) {
    return x == 1 ? 1 : 1 - std::pow(2, -10 * x);
}

[[nodiscard, maybe_unused]] static float ease_in_out_cubic(float x) {
    return x < 0.5 ? 4 * std::pow(x, 3) : 1 - std::pow(-2 * x + 2, 3) / 2;
}

[[nodiscard, maybe_unused]] static float ease_in_out_back(float x) {
    float c1 = 1.70158;
    float c2 = c1 * 1.525;

    return x < 0.5
    ? (std::pow(2 * x, 2) * ((c2 + 1) * 2 * x - c2)) / 2
    : (std::pow(2 * x - 2, 2) * ((c2 + 1) * (x * 2 - 2) + c2) + 2) / 2;
}

}




template <typename T> requires std::is_arithmetic_v<T>
class Interpolator {
    using InterpFn = std::function<float(float)>;
    const InterpFn m_fn;

public:
    const T m_start;
    const T m_end;
    const double m_duration;

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

    [[nodiscard]] operator T() const {
        return get();
    }

    [[nodiscard]] T get(double t) const {
        double x = t / m_duration;
        // TODO: custom lerp for arbitrary types
        return std::lerp(m_start, m_end, m_fn(x));
    }

};






// TODO: pause/resume semantics?

template <typename T> requires std::is_arithmetic_v<T>
class Animation {
    const std::vector<Interpolator<T>> m_interps;
    double m_start_time = 0.0f;
    enum class State { Stopped, Running } m_state = State::Stopped;

public:
    Animation(std::initializer_list<Interpolator<T>> interps)
    : m_interps(interps)
    { }

    void start() {
        m_state = State::Running;
        m_start_time = get_time_secs();
    }

    void reset() {
        m_state = State::Stopped;
        m_start_time = 0.0f;
    }

    [[nodiscard]] bool is_done() const {
        return get_time() >= get_duration();
    }

    [[nodiscard]] T get() const {
        switch (m_state) {
            case State::Running:
                return is_done() ? m_interps.back().m_end : get_running();

            case State::Stopped:
                return m_interps.front().m_start;
        }
    }

    [[nodiscard]] double get_duration() const {

        auto acc_fn = [](double acc, const anim::Interpolator<float> &b) {
            return acc + b.m_duration;
        };

        return std::accumulate(m_interps.cbegin(), m_interps.cend(), 0.0f, acc_fn);
    }

    [[nodiscard]] operator T() const {
        return get();
    }

private:
    [[nodiscard]] double get_time() const {
        return get_time_secs() - m_start_time;
    }

    [[nodiscard]] T get_running() const {

        double t = get_time();
        double time_to_interp = 0.0f;

        auto find_fn = [&](const Interpolator<float> &interp) {
            time_to_interp += interp.m_duration;
            bool is_current = t <= time_to_interp;
            return is_current;
        };

        auto current = std::ranges::find_if(m_interps, find_fn);
        assert(current != m_interps.end());

        double diff = time_to_interp - t;
        return current->get(current->m_duration - diff);
    }

    [[nodiscard]] static inline double get_time_secs() {
        namespace chrono = std::chrono;

        auto now = chrono::steady_clock::now();
        auto time = now.time_since_epoch();
        return chrono::duration_cast<chrono::duration<double>>(time).count();
    }

};

}
