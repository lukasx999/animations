#include <print>

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

    while (!WindowShouldClose()) {
        BeginDrawing();
        {
            ClearBackground(BLACK);

            float x = 500;
            DrawLineEx({x, start}, {x, end}, 10, GRAY);

            if (!IsKeyDown(KEY_J)) {
                DrawCircleV({x, y1}, radius, BLUE);
            }

            if (IsKeyPressed(KEY_K))
                y1.start();

            if (IsKeyPressed(KEY_O))
                y1.reset();

        }
        EndDrawing();
    }

    CloseWindow();

    return EXIT_SUCCESS;
}
