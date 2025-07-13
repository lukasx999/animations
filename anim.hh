#pragma once

#include <print>
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





// TODO: pause/resume semantics?
struct IAnimation {
    virtual void start() = 0;
    virtual void reset() = 0;
    [[nodiscard]] virtual double get_duration() const = 0;
    [[nodiscard]] virtual bool is_stopped() const = 0;
    [[nodiscard]] virtual bool is_done() const = 0;
    [[nodiscard]] virtual bool is_running() const = 0;
    virtual ~IAnimation() = default;
};











template <typename T>
T lerp(T start, T end, float x) = delete;

template <typename T> requires std::is_arithmetic_v<T>
[[nodiscard]] inline constexpr T lerp(T start, T end, float x) {
    return start + x * (end - start);
}

// concept for a type that may be interpolated
// custom types may be added via template specialization of anim::lerp()

template <typename T>
concept Interpolatable = requires (T start, T end, float x) {
    lerp(start, end, x);
};

// TODO: implement IAnimation for this?

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
    enum class State { Stopped, Running };

    const std::vector<Interpolator<T>> m_interps;
    double m_start_time = 0.0f;
    State m_state = State::Stopped;

public:
    Animation(std::initializer_list<Interpolator<T>> interps)
    : m_interps(interps)
    { }

    void start() override {
        m_state = State::Running;
        m_start_time = get_time_secs();
    }

    void reset() override {
        m_state = State::Stopped;
        m_start_time = 0.0f;
    }

    [[nodiscard]] double get_duration() const override {

        auto fn = [](double acc, anim::Interpolator<T> const& interp) {
            return acc + interp.get_duration();
        };

        return std::accumulate(m_interps.cbegin(), m_interps.cend(), 0.0f, fn);
    }

    [[nodiscard]] bool is_stopped() const override {
        return m_state == State::Stopped;
    }

    [[nodiscard]] bool is_running() const override {
        return m_state == State::Running;
    }

    [[nodiscard]] bool is_done() const override {
        if (is_stopped()) return false;
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
        switch (m_state) {
            case State::Running: {
                if (is_done()) return m_interps.back().get_end();
                double t = get_time();
                return get(t);
            }

            case State::Stopped:
                return m_interps.front().get_start();
        }
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



// runs animations concurrently
class Batch : public IAnimation {
    std::vector<std::reference_wrapper<IAnimation>> m_anims;
    using Iterator = decltype(m_anims)::iterator;
    using ConstIterator = decltype(m_anims)::const_iterator;

public:
    Batch(std::initializer_list<std::reference_wrapper<IAnimation>> anims)
    : m_anims(anims)
    { }

    template <Interpolatable T>
    [[nodiscard]] Animation<T>& get(std::size_t idx) {
        auto ptr = dynamic_cast<Animation<T>*>(&m_anims[idx].get());
        assert(ptr != nullptr);
        return *ptr;
    }

    void start() override {
        for (auto& anim : m_anims)
            anim.get().start();
    }

    void reset() override {
        for (auto& anim : m_anims)
            anim.get().reset();
    }

    [[nodiscard]] double get_duration() const override {
        return get_longest().get().get_duration();
    }

    [[nodiscard]] bool is_stopped() const override {
        return get_longest().get().is_stopped();
    }

    [[nodiscard]] bool is_done() const override {
        return get_longest().get().is_done();
    }

    [[nodiscard]] bool is_running() const override {
        return get_longest().get().is_running();
    }

    [[nodiscard]] Iterator begin() {
        return m_anims.begin();
    }

    [[nodiscard]] Iterator end() {
        return m_anims.end();
    }

    [[nodiscard]] ConstIterator cbegin() const {
        return m_anims.cbegin();
    }

    [[nodiscard]] ConstIterator cend() const {
        return m_anims.cend();
    }

private:
    [[nodiscard]] std::reference_wrapper<IAnimation> const& get_longest() const {
        auto max_fn = [](std::reference_wrapper<IAnimation> const& a, decltype(a) b) {
            return b.get().get_duration() > a.get().get_duration();
        };

        auto longest = std::ranges::max_element(m_anims, max_fn);

        assert(longest != m_anims.end());

        return *longest;
    }

};

// runs animations synchronously
class Sequence : public IAnimation {
    std::vector<std::reference_wrapper<IAnimation>> m_anims;
    std::optional<decltype(m_anims)::iterator> m_current;

public:
    Sequence(std::initializer_list<std::reference_wrapper<IAnimation>> anims)
    : m_anims(anims)
    { }

    void dispatch() {

        bool running = m_current.has_value();
        if (!running) return;

        auto& it = m_current.value();

        if (it->get().is_done()) {
            it++;

            bool is_at_end = m_current == m_anims.end();
            if (is_at_end) {
                m_current = { };
                return;
            }

            it->get().start();
        }

    }

    void start() override {
        reset();
        m_anims.front().get().start();
        m_current = m_anims.begin();
    }

    void reset() override {
        for (auto& anim : m_anims)
            anim.get().reset();
        m_current = { };
    }

    [[nodiscard]] double get_duration() const override {
        auto fn = [](double acc, std::reference_wrapper<IAnimation> const& elem) {
            return acc + elem.get().get_duration();
        };
        double t = std::accumulate(m_anims.cbegin(), m_anims.cend(), 0.0f, fn);
        return t;
    }

    [[nodiscard]] bool is_stopped() const override {
        return m_anims.front().get().is_done();
    }

    [[nodiscard]] bool is_done() const override {
        return m_anims.back().get().is_done();
    }

    [[nodiscard]] bool is_running() const override {
        auto fn = [](std::reference_wrapper<IAnimation> const& elem) {
            return elem.get().is_running();
        };

        return std::ranges::any_of(m_anims, fn);
    }

};

}
