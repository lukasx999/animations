#include <print>

#include "raylib-cpp.hpp"

constexpr int WIDTH  = 1600;
constexpr int HEIGHT = 900;

class Animation {
    bool m_active = false;

protected:
    float m_t = 0;
    float m_duration;

public:
    Animation(float duration)
        : m_duration(duration)
    { }

    void start() {
        m_active = true;
    }

    [[nodiscard]] bool is_active() const {
        return m_active;
    }

    float poll() {

        if (m_active) {

            float x =  interpolate();
            m_t += GetFrameTime();

            if (m_t > m_duration) {
                m_t = 0;
                m_active = false;
            }

            return x;

        } else {
            return 0;

        }
    }

protected:
    [[nodiscard]] virtual float interpolate() const = 0;

};

class LinearAnimation : public Animation {
public:

    LinearAnimation(float duration) : Animation(duration) { }

protected:
    [[nodiscard]] float interpolate() const {
        return m_t / m_duration;
    }

};

class SinAnimation : public LinearAnimation {
    float m_sin_x;

public:

    SinAnimation(float duration, float sin_x=PI*2)
        : LinearAnimation(duration)
        , m_sin_x(sin_x)
    { }

protected:
    [[nodiscard]] float interpolate() const {
        return sin(m_sin_x * LinearAnimation::interpolate());
    }

};




int main() {

    raylib::Window window(WIDTH, HEIGHT, "anim", FLAG_WINDOW_RESIZABLE);
    SetTargetFPS(60);

    SinAnimation anim(1, PI/2);

    while (!window.ShouldClose()) {
        BeginDrawing();
        {

            window.ClearBackground(BLACK);

            float a = anim.poll();
            int size = 100;
            int size_diff = a * 100;
            int outer_size = size + size_diff;

            float speed = 2;
            double interp = (sin(GetTime() * speed) + 1) / 2;

            auto eps = 1e-2;
            if (interp <= eps || 1 - interp <= eps) {
                anim.start();
            }

            raylib::Rectangle outer(
                (WIDTH-size) * interp - size_diff/2.0,
                HEIGHT / 2.0 - outer_size / 2.0,
                outer_size,
                outer_size
            );

            Color c = PURPLE;
            c.a = Lerp(200, 0, a);
            outer.Draw(c);

            raylib::Rectangle rect(
                (WIDTH-size) * interp,
                HEIGHT / 2.0 - size / 2.0,
                size,
                size
            );
            rect.Draw(BLUE);


        }
        EndDrawing();
    }

    return 0;
}
