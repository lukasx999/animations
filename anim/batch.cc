#include "batch.hh"

#include <algorithm>

namespace anim {


Batch::Batch(std::initializer_list<std::reference_wrapper<IAnimation>> anims)
: m_anims(anims)
{ }

Batch::Batch(IAnimation& anim) : m_anims({ anim }) { }

void Batch::add(IAnimation& anim) {
    m_anims.push_back(anim);
}

void Batch::start() {
    for (auto& anim : m_anims)
        anim.get().start();
}

void Batch::reset() {
    for (auto& anim : m_anims)
        anim.get().reset();
}

[[nodiscard]] double Batch::get_progress() const {
    return get_longest().get().get_progress();
}

[[nodiscard]] double Batch::get_duration() const {
    return get_longest().get().get_duration();
}

[[nodiscard]] bool Batch::is_stopped() const {
    return get_longest().get().is_stopped();
}

[[nodiscard]] bool Batch::is_done() const {
    return get_longest().get().is_done();
}

[[nodiscard]] bool Batch::is_running() const {
    return get_longest().get().is_running();
}

[[nodiscard]] std::reference_wrapper<IAnimation> const& Batch::get_longest() const {

    auto max_fn = [](std::reference_wrapper<IAnimation> const& a, decltype(a) b) {
        return b.get().get_duration() > a.get().get_duration();
    };

    auto longest = std::ranges::max_element(m_anims, max_fn);

    assert(longest != m_anims.end());

    return *longest;
}


}
