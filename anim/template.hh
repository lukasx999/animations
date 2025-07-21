#pragma once

#include "anim.hh"

namespace anim {


class AnimationTemplate : public IAnimation {
protected:
    Sequence m_anim;
    AnimationTemplate() = default;
    AnimationTemplate(Sequence anim) : m_anim(anim) { }
    virtual void on_update() { }

public:
    void update() {
        m_anim.dispatch();
        on_update();
    }

    void start() override {
        m_anim.start();
    }

    void reset() override {
        m_anim.reset();
    }

    [[nodiscard]] double get_progress() const override {
        return m_anim.get_progress();
    }

    [[nodiscard]] double get_duration() const override {
        return m_anim.get_duration();
    }

    [[nodiscard]] bool is_stopped() const override {
        return m_anim.is_stopped();
    }

    [[nodiscard]] bool is_done() const override {
        return m_anim.is_done();
    }

    [[nodiscard]] bool is_running() const override {
        return m_anim.is_running();
    }

};


}
