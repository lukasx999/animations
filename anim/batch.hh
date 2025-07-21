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
    Batch(std::initializer_list<std::reference_wrapper<IAnimation>> anims);
    Batch(IAnimation& anim);

    void add(IAnimation& anim);
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
