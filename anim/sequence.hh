#pragma once

#include <vector>
#include <optional>

#include "common.hh"

namespace anim {

// runs animations synchronously
class Sequence : public IAnimation {
    std::vector<std::reference_wrapper<IAnimation>> m_anims;
    std::optional<decltype(m_anims)::iterator> m_current;

public:
    Sequence() = default;
    Sequence(std::initializer_list<std::reference_wrapper<IAnimation>> anims);
    Sequence(IAnimation& anim);

    void add(IAnimation& anim);
    void dispatch();
    void start() override;
    void reset() override;
    [[nodiscard]] double get_progress() const override;
    [[nodiscard]] double get_duration() const override;
    [[nodiscard]] bool is_stopped() const override;
    [[nodiscard]] bool is_done() const override;
    [[nodiscard]] bool is_running() const override;

};

}
