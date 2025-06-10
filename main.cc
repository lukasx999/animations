#include <print>
#include <functional>

#include "raylib-cpp.hpp"

constexpr int WIDTH  = 1600;
constexpr int HEIGHT = 800;

typedef std::function<float(float)> Interpolator;

class Animation {
    bool m_active = false;
    float m_t = 0;
    const float m_duration;
    Interpolator m_interp;

public:
    Animation(float duration, Interpolator interp)
        : m_duration(duration)
        , m_interp(interp)
    { }

    void start() {
        m_active = true;
    }

    void reset() {
        m_active = false;
        m_t = 0;
    }

    [[nodiscard]] bool is_active() const {
        return m_active;
    }

    [[nodiscard]] float peek() const {
        return m_interp(m_t / m_duration);
    }

    float update(float dt) {

        if (m_active) {
            m_t += dt;
            // interpolation should happen before reset, as otherwise update()
            // will briefly return 0, when time has reached duration
            float x = peek();

            if (m_t > m_duration)
                reset();

            return x;

        } else {
            return 1;

        }
    }

};

[[nodiscard]] float interp_lin(float x) {
    return x;
}

[[nodiscard]] float interp_sin(float x) {
    return sin((PI/2) * x);
}

[[nodiscard]] float interp_sq(float x) {
    return pow(x, 2);
}

[[nodiscard]] float interp_cub(float x) {
    return pow(x, 3);
}

[[nodiscard]] float interp_ease_out_back(float x) {
    float c1 = 1.70158;
    float c3 = c1 + 1;
    return 1 + c3 * pow(x-1, 3) + c1 * pow(x-1, 2);
}

static void animate_circle(Animation &anim, Color color, int offset) {

    int r = 25;
    float linx = anim.update(GetFrameTime());
    int pad = 200;

    raylib::Vector2 start(pad+r, offset);
    raylib::Vector2 end(WIDTH-pad-r, offset);
    start.DrawLine(end, WHITE);
    start.DrawCircle(5, WHITE);
    end.DrawCircle(5, WHITE);

    raylib::Vector2 circ_lin(pad + r + (WIDTH-2*r-2*pad) * linx, offset);
    circ_lin.DrawCircle(r, color);
}



int main() {

    raylib::Window window(WIDTH, HEIGHT, "anim", FLAG_WINDOW_RESIZABLE);
    SetTargetFPS(60);

    Animation lina(3, interp_lin);
    lina.start();

    Animation sqa(3, interp_sq);
    sqa.start();

    Animation cua(3, interp_cub);
    cua.start();

    Animation sina(3, interp_sin);
    sina.start();

    Animation easa(3, interp_ease_out_back);
    easa.start();

    while (!window.ShouldClose()) {
        BeginDrawing();
        {
            window.ClearBackground(BLACK);

            animate_circle(lina, BLUE,   100);
            animate_circle(sqa,  YELLOW, 200);
            animate_circle(cua,  ORANGE, 300);
            animate_circle(sina, RED,    400);
            animate_circle(easa, PURPLE, 500);

        }
        EndDrawing();
    }

    return 0;
}
