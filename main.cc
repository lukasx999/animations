#include <print>
#include <functional>
#include <chrono>
#include <cmath>

#include <raylib.h>

#define PRINT(x) std::println("{}: {}", #x, x);

static constexpr auto WIDTH = 1600;
static constexpr auto HEIGHT = 900;

template <typename T> requires std::is_arithmetic_v<T>
class Interpolator {
    using Fn = std::function<float(float)>;

    const T m_start;
    const T m_end;
    const double m_duration;
    const Fn m_fn;
    double m_start_time = get_time_secs();

public:
    Interpolator() : Interpolator(0.0f, 1.0f) { }

    Interpolator(T start, T end) : Interpolator(start, end, 1.0f) { }

    Interpolator(T start, T end, double duration)
    : Interpolator(start, end, duration, [](float x) { return x; })
    { }

    Interpolator(T start, T end, double duration, Fn f)
        : m_start(start)
        , m_end(end)
        , m_duration(duration)
        , m_fn(f)
    { }

    void reset() {
        m_start_time = get_time_secs();
    }

    [[nodiscard]] bool is_done() const {
        return get_time_secs() >= m_start_time + m_duration;
    }

    [[nodiscard]] T get() const {
        if (is_done()) return m_end;

        double t = get_time_secs() - m_start_time;
        double x = t / m_duration;
        return std::lerp(m_start, m_end, m_fn(x));
    }

    [[nodiscard]] operator T() const {
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

[[nodiscard]] static float linear(float x) {
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

[[nodiscard]] static float ease_in_out_cubic(float x) {
    return x < 0.5 ? 4 * std::pow(x, 3) : 1 - std::pow(-2 * x + 2, 3) / 2;
}

}

int main() {

    InitWindow(WIDTH, HEIGHT, "animations");
    SetTargetFPS(60);

    float width = 100, height = width;
    Interpolator<float> x(0, WIDTH-width*2, 1, easings::ease_in_out_cubic);
    Interpolator<float> delta(0, 50, 0.5, easings::ease_out_expo);
    Interpolator<float> delta_color(0, 1.0f, 0.3, easings::squared);
    bool start = false;

    while (!WindowShouldClose()) {
        BeginDrawing();
        {
            ClearBackground(BLACK);

            float y = HEIGHT/2.0f - height/2.0f;

            Rectangle rect { x, y, width, height };
            DrawRectangleRec(rect, BLUE);

            if (x.is_done()) {

                if (!start) {
                    delta.reset();
                    delta_color.reset();
                    start = true;
                }

                Rectangle rect { x-delta, y-delta, width+delta*2, height+delta*2 };
                auto color = ColorLerp(RED, BLACK, delta_color);
                DrawRectangleRec(rect, color);
            }

        }
        EndDrawing();
    }

    CloseWindow();

    return EXIT_SUCCESS;
}
