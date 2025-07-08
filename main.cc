#include <numeric>
#include <print>
#include <functional>
#include <chrono>
#include <cmath>
#include <utility>

#include <raylib.h>
#include <raymath.h>

#include "anim.hh"

#define PRINT(x) std::println("{}: {}", #x, x);

static constexpr auto WIDTH = 1600;
static constexpr auto HEIGHT = 900;



int main() {

    InitWindow(WIDTH, HEIGHT, "animations");
    SetTargetFPS(60);

    float radius = 50;

    float offset = 300;

    float start = HEIGHT/2.0f-offset;
    float end = HEIGHT/2.0f+offset;
    anim::Animation<float> y1 {
        { start, end, 3, anim::interpolators::ease_in_out_back },
    };

    y1.start();

    while (!WindowShouldClose()) {
        BeginDrawing();
        {
            ClearBackground(BLACK);
            DrawText(std::format("{}", y1.get_time()).c_str(), 0, 0, 50, WHITE);
            DrawText(std::format("{}", y1.is_running()).c_str(), 0, 50, 50, WHITE);

            float x = 500;
            DrawLineEx({x, start}, {x, end}, 10, GRAY);
            if (!IsKeyDown(KEY_J)) {
                DrawCircleV({x, y1}, radius, BLUE);
            }

            if (IsKeyPressed(KEY_K))
                y1.start();

        }
        EndDrawing();
    }

    CloseWindow();

    return EXIT_SUCCESS;
}
