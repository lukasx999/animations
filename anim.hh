#pragma once

#include <functional>
#include <chrono>
#include <cmath>
#include <utility>

namespace easings {

[[nodiscard, maybe_unused]] static float step([[maybe_unused]] float x) {
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

}

[[nodiscard]] inline double get_time_secs() {
    namespace chrono = std::chrono;

    auto now = chrono::steady_clock::now();
    auto time = now.time_since_epoch();
    return chrono::duration_cast<chrono::duration<double>>(time).count();
}

// TODO: pause/resume semantics?

template <typename T> requires std::is_arithmetic_v<T>
class Interpolator {
    using InterpFn = std::function<float(float)>;
    double m_start_time = 0.0f;
    enum class State {
        Idle,
        Running,
        Done,
    } mutable m_state = State::Idle;

public:
    const T m_start;
    const T m_end;
    // const double m_duration;
    double m_duration;
    const InterpFn m_fn;

    Interpolator() : Interpolator(1.0f) { }

    explicit Interpolator(T end) : Interpolator(0.0f, end) { }

    Interpolator(T start, T end) : Interpolator(start, end, 1.0f) { }

    Interpolator(T start, T end, double duration)
    : Interpolator(start, end, duration, [](float x) { return x; })
    { }

    Interpolator(T start, T end, double duration, InterpFn f)
        : m_start(start)
        , m_end(end)
        , m_duration(duration)
        , m_fn(f)
    { }

    void start() {
        m_state = State::Running;
        m_start_time = get_time_secs();
    }

    void reset() {
        m_state = State::Idle;
        m_start_time = 0.0f;
    }

    [[nodiscard]] bool is_running() const {
        return m_state == State::Running;
    }

    [[nodiscard]] bool is_done() const {
        return m_state == State::Done;
    }

    [[nodiscard]] operator T() const {
        return get();
    }

    [[nodiscard]] T get(double t) const {
        double x = t / m_duration;
        // TODO: custom lerp for arbitrary types
        return std::lerp(m_start, m_end, m_fn(x));
    }

    [[nodiscard]] T get() const {

        switch (m_state) {

            case State::Running: {

                if (has_ended()) {
                    m_state = State::Done;
                    return get();
                }

                double t = get_time_secs() - m_start_time;
                return get(t);
            } break;

            case State::Idle:
                return m_start;
                break;

            case State::Done:
                return m_end;
                break;
        }

        std::unreachable();

    }

private:
    [[nodiscard]] bool has_ended() const {
        return get_time_secs() >= m_start_time + m_duration;
    }

};
