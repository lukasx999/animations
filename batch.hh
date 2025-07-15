#pragma once

#include <algorithm>
#include <cassert>
#include <vector>
#include "animation.hh"

namespace anim {

// runs animations concurrently
class Batch : public IAnimation {
    std::vector<std::reference_wrapper<IAnimation>> m_anims;
    using Iterator = decltype(m_anims)::iterator;
    using ConstIterator = decltype(m_anims)::const_iterator;

public:
    Batch() = default;

    Batch(std::initializer_list<std::reference_wrapper<IAnimation>> anims)
    : m_anims(anims)
    { }

    Batch(IAnimation& anim) : m_anims({ anim }) { }

    void add(IAnimation& anim) {
        m_anims.push_back(anim);
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

}
