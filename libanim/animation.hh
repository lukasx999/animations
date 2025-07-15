#pragma once

#include <vector>
#include <chrono>
#include <numeric>

#include "common.hh"

namespace anim {

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

    [[nodiscard]] static double get_time_secs() {
        namespace chrono = std::chrono;

        auto now = chrono::steady_clock::now();
        auto time = now.time_since_epoch();
        return chrono::duration_cast<chrono::duration<double>>(time).count();
    }

};

}
