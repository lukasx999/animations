#include "batch.hh"

#include <algorithm>

namespace anim {


[[nodiscard]] std::reference_wrapper<IAnimation> const& Batch::get_longest() const {

    auto max_fn = [](std::reference_wrapper<IAnimation> const& a, decltype(a) b) {
        return b.get().get_duration() > a.get().get_duration();
    };

    auto longest = std::ranges::max_element(m_anims, max_fn);

    assert(longest != m_anims.end());

    return *longest;
}



}
