#include <raylib.h>
#include <raymath.h>

#define ANIM_INTEGRATION_RAYLIB
#include <anim.hh>

static constexpr int WIDTH = 1600;
static constexpr int HEIGHT = 900;

int main() {

    InitWindow(WIDTH, HEIGHT, "animations");
    SetTargetFPS(180);

    float offset = 300;
    Vector2 start = { offset, HEIGHT/2.0f };
    Vector2 end = { WIDTH-offset, HEIGHT/2.0f };
    anim::Animation<Vector2> pos({ start, end, 2, anim::interpolators::ease_in_out_back });

    pos.start();

    while (!WindowShouldClose()) {
        BeginDrawing();
            ClearBackground(BLACK);
            DrawCircleV(pos, 50, BLUE);
        EndDrawing();
    }

    CloseWindow();

    return EXIT_SUCCESS;
}
