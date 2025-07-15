#pragma once

#include <vector>
#include <optional>
#include <numeric>
#include <algorithm>

#include "animation.hh"

namespace anim {

// runs animations synchronously
class Sequence : public IAnimation {
    std::vector<std::reference_wrapper<IAnimation>> m_anims;
    std::optional<decltype(m_anims)::iterator> m_current;

public:
    Sequence() = default;

    Sequence(std::initializer_list<std::reference_wrapper<IAnimation>> anims)
    : m_anims(anims)
    { }

    Sequence(IAnimation& anim) : m_anims({ anim }) { }

    void add(IAnimation& anim) {
        m_anims.push_back(anim);
    }

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
