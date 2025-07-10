#pragma once

#include <cassert>
#include <algorithm>
#include <functional>
#include <chrono>
#include <cmath>
#include <numeric>

namespace anim {

namespace interpolators {

[[nodiscard]] inline constexpr float step(float) {
    return 1.0f;
}

[[nodiscard]] inline constexpr float linear(float x) {
    return x;
}

[[nodiscard]] inline constexpr float squared(float x) {
    return std::pow(x, 2);
}

[[nodiscard]] inline constexpr float cubed(float x) {
    return std::pow(x, 3);
}

[[nodiscard]] inline constexpr float ease_out_expo(float x) {
    return x == 1 ? 1 : 1 - std::pow(2, -10 * x);
}

[[nodiscard]] inline constexpr float ease_in_out_cubic(float x) {
    return x < 0.5 ? 4 * std::pow(x, 3) : 1 - std::pow(-2 * x + 2, 3) / 2;
}

[[nodiscard]] inline constexpr float ease_in_out_back(float x) {
    float c1 = 1.70158;
    float c2 = c1 * 1.525;

    return x < 0.5
    ? (std::pow(2 * x, 2) * ((c2 + 1) * 2 * x - c2)) / 2
    : (std::pow(2 * x - 2, 2) * ((c2 + 1) * (x * 2 - 2) + c2) + 2) / 2;
}

}





// TODO: probably not a good idea, beacuse of generics
class IAnimation {
public:
    virtual void start() = 0;
    virtual void reset() = 0;
    [[nodiscard]] virtual double get_duration() const = 0;
    [[nodiscard]] virtual bool is_stopped() const = 0;
    [[nodiscard]] virtual bool is_done() const = 0;
    [[nodiscard]] virtual bool is_running() const = 0;
};











template <typename T>
T lerp(T start, T end, float x) = delete;

template <typename T> requires std::is_arithmetic_v<T>
[[nodiscard]] inline constexpr T lerp(T start, T end, float x) {
    return start + x * (end - start);
}

// Marks a type, that may be interpolated.
// Types may be qualified via template specialization.

template <typename T>
concept Interpolatable = requires (T start, T end, float x) {
    lerp(start, end, x);
};

template <Interpolatable T>
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
        return anim::lerp(m_start, m_end, m_fn(x));
    }

};






// TODO: pause/resume semantics?

template <Interpolatable T>
class Animation {
    const std::vector<Interpolator<T>> m_interps;
    double m_start_time = 0.0f;
    enum class State { Stopped, Running };
    State m_state = State::Stopped;

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

    [[nodiscard]] bool is_stopped() const {
        return m_state == State::Stopped;
    }

    [[nodiscard]] bool is_running() const {
        return m_state == State::Running;
    }

    [[nodiscard]] bool is_done() const {
        if (is_stopped()) return false;
        return get_time() >= get_duration();
    }

    [[nodiscard]] T get(float t) const {

        double time_to_interp = 0.0f;

        auto find_fn = [&](const Interpolator<T> &interp) {
            time_to_interp += interp.m_duration;
            bool is_current = t <= time_to_interp;
            return is_current;
        };

        auto current = std::ranges::find_if(m_interps, find_fn);
        assert(current != m_interps.end());

        double diff = time_to_interp - t;
        return current->get(current->m_duration - diff);
    }

    [[nodiscard]] T get() const {
        switch (m_state) {
            case State::Running: {
                if (is_done()) return m_interps.back().m_end;
                double t = get_time();
                return get(t);
            }

            case State::Stopped:
                return m_interps.front().m_start;
        }
    }

    [[nodiscard]] double get_duration() const {

        auto acc_fn = [](double acc, const anim::Interpolator<T> &interp) {
            return acc + interp.m_duration;
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

    [[nodiscard]] static inline double get_time_secs() {
        namespace chrono = std::chrono;

        auto now = chrono::steady_clock::now();
        auto time = now.time_since_epoch();
        return chrono::duration_cast<chrono::duration<double>>(time).count();
    }

};




template <Interpolatable T>
class Batch {
    std::vector<Animation<T>> m_anims;
    using Iterator = std::vector<Animation<T>>::iterator;

public:
    Batch(std::initializer_list<Animation<T>> anims)
    : m_anims(anims)
    { }

    [[nodiscard]] double get_duration() const {
        return get_longest().get_duration();
    }

    [[nodiscard]] bool is_done() const {
        return get_longest().is_done();
    }

    [[nodiscard]] bool is_running() const {
        return get_longest().is_running();
    }

    [[nodiscard]] bool is_stopped() const {
        return get_longest().is_stopped();
    }

    void start() {
        for (auto &anim : m_anims)
            anim.start();
    }

    void reset() {
        for (auto &anim : m_anims)
            anim.reset();
    }

    [[nodiscard]] inline Iterator begin() {
        return m_anims.begin();
    }

    [[nodiscard]] inline Iterator end() {
        return m_anims.end();
    }

    [[nodiscard]] const Animation<T> &operator[](std::size_t idx) const {
        return m_anims[idx];
    }

private:
    [[nodiscard]] const Animation<T> &get_longest() const {

        auto longest = std::ranges::max_element(m_anims, [](const Animation<T> &a, const Animation<T> &b) {
            return b.get_duration() > a.get_duration();
        });

        assert(longest != m_anims.end());

        return *longest;
    }


};



// TODO: run batches synchronously

template <Interpolatable T>
class Sequence {
    std::vector<Batch<T>> m_batches;

public:
    Sequence(std::initializer_list<Batch<T>> batches) : m_batches(batches) { }

};




}
