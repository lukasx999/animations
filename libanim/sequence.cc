#include <numeric>
#include <algorithm>

#include "sequence.hh"

namespace anim {

void Sequence::dispatch() {
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

void Sequence::start() {
    reset();
    m_anims.front().get().start();
    m_current = m_anims.begin();
}

void Sequence::reset() {
    for (auto& anim : m_anims)
    anim.get().reset();
    m_current = { };
}

[[nodiscard]] double Sequence::get_duration() const {
    auto fn = [](double acc, std::reference_wrapper<IAnimation> const& elem) {
        return acc + elem.get().get_duration();
    };
    double t = std::accumulate(m_anims.cbegin(), m_anims.cend(), 0.0f, fn);
    return t;
}

[[nodiscard]] bool Sequence::is_stopped() const {
    return m_anims.front().get().is_done();
}

[[nodiscard]] bool Sequence::is_done() const {
    return m_anims.back().get().is_done();
}

[[nodiscard]] bool Sequence::is_running() const {
    auto fn = [](std::reference_wrapper<IAnimation> const& elem) {
        return elem.get().is_running();
    };

    return std::ranges::any_of(m_anims, fn);
}


}
