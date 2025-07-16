#pragma once

#include <cassert>
#include <vector>

#include "common.hh"

namespace anim {

// runs animations concurrently
class Batch : public IAnimation {
    std::vector<std::reference_wrapper<IAnimation>> m_anims;

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

    [[nodiscard]] double get_progress() const override {
        return get_longest().get().get_progress();
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

private:
    [[nodiscard]] std::reference_wrapper<IAnimation> const& get_longest() const;

};

}
