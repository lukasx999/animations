#include <print>
#include <functional>
#include <chrono>
#include <cmath>

#include <raylib.h>

#define PRINT(x) std::println("{}: {}", #x, x);

static constexpr auto WIDTH = 1600;
static constexpr auto HEIGHT = 900;

class Interpolated {
    using F = std::function<float(float)>;

    const float m_start;
    const float m_end;
    const float m_duration;
    const F m_f;
    const double m_start_time = get_time_secs();

public:
    Interpolated()
    : Interpolated(0.0f, 1.0f)
    { }

    Interpolated(float start, float end)
    : Interpolated(start, end, 1.0f)
    { }

    Interpolated(float start, float end, float duration)
    : Interpolated(start, end, duration, [](float x) { return x; })
    { }

    Interpolated(float start, float end, float duration, F f)
        : m_start(start)
        , m_end(end)
        , m_duration(duration)
        , m_f(f)
    { }

    [[nodiscard]] float get() const {
        if (get_time_secs() >= m_start_time + m_duration)
            return m_end;

        auto x = (get_time_secs() - m_start_time) / m_duration;
        return std::lerp(m_start, m_end, m_f(x));
    }

    [[nodiscard]] operator float() const {
        return get();
    }

private:
    [[nodiscard]] static double get_time_secs() {
        namespace chrono = std::chrono;

        auto now = chrono::steady_clock::now();
        auto time = now.time_since_epoch();
        return chrono::duration_cast<chrono::duration<double>>(time).count();
    }

};

namespace easings {

[[nodiscard]] static float id(float x) {
    return x;
}

[[nodiscard]] static float squared(float x) {
    return std::pow(x, 2);
}

[[nodiscard]] static float cubed(float x) {
    return std::pow(x, 3);
}

[[nodiscard]] static float ease_out_expo(float x) {
    return x == 1 ? 1 : 1 - std::pow(2, -10 * x);
}

}

int main() {

    InitWindow(WIDTH, HEIGHT, "raylib [core] example - basic window");
    SetTargetFPS(60);

    float width = 100, height = width;
    Interpolated x(0, WIDTH-width, 2, easings::ease_out_expo);

    while (!WindowShouldClose()) {
        BeginDrawing();
        {
            ClearBackground(BLACK);

            // float x = std::lerp(0, WIDTH-width, fmod(GetTime(), 1));
            float y = 0;

            Rectangle rect { x, y, width, height };
            DrawRectangleRec(rect, BLUE);

        }
        EndDrawing();
    }

    CloseWindow();

    return EXIT_SUCCESS;
}
