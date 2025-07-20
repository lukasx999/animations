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

    void start() override;
    void reset() override;
    [[nodiscard]] double get_progress() const override;
    [[nodiscard]] double get_duration() const override;
    [[nodiscard]] bool is_stopped() const override;
    [[nodiscard]] bool is_done() const override;
    [[nodiscard]] bool is_running() const override;

private:
    [[nodiscard]] std::reference_wrapper<IAnimation> const& get_longest() const;

};

}
