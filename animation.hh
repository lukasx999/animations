#pragma once

namespace anim {

// TODO: pause/resume semantics?
struct IAnimation {
    virtual void start() = 0;
    virtual void reset() = 0;
    [[nodiscard]] virtual double get_duration() const = 0;
    [[nodiscard]] virtual bool is_stopped() const = 0;
    [[nodiscard]] virtual bool is_done() const = 0;
    [[nodiscard]] virtual bool is_running() const = 0;
    virtual ~IAnimation() = default;
};

}
