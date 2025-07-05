#include <print>
#include <functional>
#include <chrono>
#include <cmath>

#include <raylib.h>

#define PRINT(x) std::println("{}: {}", #x, x);

static constexpr auto WIDTH = 1600;
static constexpr auto HEIGHT = 900;

class Interpolated {
    float m_start;
    float m_end;
    float m_duration;
    double m_start_time = get_time_secs();
    using F = std::function<float(float)>;
    F m_f;

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
        auto x = std::chrono::system_clock::now().time_since_epoch();
        auto t = std::chrono::duration_cast<std::chrono::milliseconds>(x).count();
        return static_cast<double>(t) / 1000;
    }

};

auto main() -> int {

    InitWindow(WIDTH, HEIGHT, "raylib [core] example - basic window");
    SetTargetFPS(60);

    float width = 100, height = width;
    Interpolated x(0, WIDTH-width, 2, [](float x) { return x*x*x; });

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
