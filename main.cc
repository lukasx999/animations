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

    anim::Animation<float> anim_x {
        { 0, 500, 1, anim::interpolators::ease_out_expo },
        { 500, WIDTH/2.0f, 2, anim::interpolators::squared },
        { WIDTH/2.0f, WIDTH-radius, 2, anim::interpolators::ease_in_out_cubic },
    };

    anim_x.start();

    while (!WindowShouldClose()) {
        BeginDrawing();
        {
            ClearBackground(BLACK);
            DrawText(std::format("{}", anim_x.get_time()).c_str(), 0, 0, 50, WHITE);
            DrawText(std::format("{}", anim_x.is_running()).c_str(), 0, 50, 50, WHITE);

            if (!IsKeyDown(KEY_J)) {
                DrawCircleV({anim_x, HEIGHT/2.0f-radius/2.0f}, radius, BLUE);
            }

        }
        EndDrawing();
    }

    CloseWindow();

    return EXIT_SUCCESS;
}
