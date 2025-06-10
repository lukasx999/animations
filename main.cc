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
    Animation(float duration) : m_duration(duration) { }

    void start() {
        m_active = true;
    }

    float poll() {

        if (m_active) {
            m_t += GetFrameTime();

            if (m_t >= m_duration) {
                m_t = 0;
                m_active = false;
            }

            return interpolate();

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
        return Lerp(0, 1, m_t);
    }

};

class SinAnimation : public Animation {
    float m_sin_x;

public:
    SinAnimation(float duration, float sin_x)
        : Animation(duration)
        , m_sin_x(sin_x)
    { }

protected:
    [[nodiscard]] float interpolate() const {
        float max = 0;
        while (max < m_duration)
            max += GetFrameTime();

        float factor = m_sin_x / max;
        return Lerp(0, 1, sin(m_t*factor));
    }

};




int main() {

    raylib::Window window(WIDTH, HEIGHT, "anim", FLAG_WINDOW_RESIZABLE);
    SetTargetFPS(60);

    SinAnimation anim(0.3, PI);

    while (!window.ShouldClose()) {
        BeginDrawing();
        {

            window.ClearBackground(BLACK);

            float a = anim.poll();
            int size = 100;
            int outer_size = size + a * 50;

            float speed = 3;
            double interp = (sin(GetTime() * speed) + 1) / 2;

            auto eps = 1e-2;

            if (1 - interp <= eps)
                anim.start();

            if (interp <= eps)
                anim.start();


            raylib::Rectangle outer(
                (WIDTH-outer_size) * interp,
                HEIGHT / 2.0 - outer_size / 2.0,
                outer_size,
                outer_size
            );
            Color c = RED;
            c.a *= a;
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
