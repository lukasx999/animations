#include <cassert>
#include <numeric>
#include <algorithm>

#include "sequence.hh"

namespace anim {


Sequence::Sequence(std::initializer_list<std::reference_wrapper<IAnimation>> anims)
: m_anims(anims)
{ }

Sequence::Sequence(IAnimation& anim)
: m_anims({ anim })
{ }

void Sequence::add(IAnimation& anim) {
    m_anims.push_back(anim);
}

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

[[nodiscard]] double Sequence::get_progress() const {

    double time_to_interp = 0.0f;

    auto fn = [&](std::reference_wrapper<IAnimation> const& interp) {
        time_to_interp += interp.get().get_duration();
        return interp.get().is_running();
    };

    auto current = std::ranges::find_if(m_anims, fn);
    assert(current != m_anims.end());

    double duration = current->get().get_duration();
    time_to_interp -= duration; // compensate overshoot

    double t = current->get().get_progress() * duration;
    double progress_abs = time_to_interp + t;

    return progress_abs / get_duration();
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
