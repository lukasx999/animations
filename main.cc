#include <print>
#include <ranges>
#include <algorithm>

#include <raylib.h>
#include <raymath.h>

#include "anim.hh"

#define PRINT(x) std::println("{}: {}", #x, x);

static constexpr auto WIDTH = 1600;
static constexpr auto HEIGHT = 900;

template <>
[[nodiscard]] inline constexpr Vector2 anim::lerp(Vector2 start, Vector2 end, float x) {
    return Vector2Lerp(start, end, x);
}

template <>
[[nodiscard]] inline constexpr Color anim::lerp(Color start, Color end, float x) {
    return ColorLerp(start, end, x);
}

int main() {

    InitWindow(WIDTH, HEIGHT, "animations");
    SetTargetFPS(180);

    float radius = 50;

    anim::Animation<float> rad {
        { 0, radius, 2, anim::interpolators::squared },
    };

    anim::Animation<Color> col {
        { RED, BLUE, 2, anim::interpolators::cubed },
    };

    float offset = 500;
    anim::Animation<Vector2> vect {
        { Vector2 { radius, radius }, Vector2 { WIDTH-radius-offset, radius }, 1, anim::interpolators::ease_in_out_cubic },
        { Vector2 { WIDTH-radius-offset, radius }, Vector2 { WIDTH-radius-offset, HEIGHT-radius-offset }, 1, anim::interpolators::ease_in_out_cubic },
    };

    rad.start();
    col.start();
    vect.start();

    while (!WindowShouldClose()) {
        BeginDrawing();
        {
            ClearBackground(BLACK);

            DrawCircleV(vect, rad, col);

        }
        EndDrawing();
    }

    CloseWindow();

    return EXIT_SUCCESS;
}



int main2() {

    InitWindow(WIDTH, HEIGHT, "animations");
    SetTargetFPS(60);

    float radius = 25;
    float offset = 100;
    float start = HEIGHT/2.0f-offset;
    float end = HEIGHT/2.0f+offset;

    std::vector<anim::Animation<float>> anims;

    int n = 14;
    for (double i=1; i <= n; ++i) {

        double duration = i < n/2.0f ? i : n-i;
        auto &anim = anims.emplace_back(anim::Animation<float> {
            { start, end, duration/2.0f, anim::interpolators::ease_in_out_cubic }
        });

        anim.start();
    }

    anim::Animation<float> foo {
        { radius, WIDTH-radius, 5, anim::interpolators::ease_out_expo }
    };

    while (!WindowShouldClose()) {
        BeginDrawing();
        {
            ClearBackground(BLACK);

            auto offset = static_cast<float>(WIDTH)/n;
            float spacing = 100;

            for (auto &&[idx, anim] : std::views::enumerate(anims)) {
                float x0 = offset + spacing * idx;
                DrawLineEx({x0, start}, {x0, end}, 5, GRAY);
                DrawCircleV({x0, anim}, radius, BLUE);
            }

            auto longest = std::ranges::max_element(anims, [](const anim::Animation<float> &a, const anim::Animation<float> &b) {
                return b.get_duration() > a.get_duration();
            });
            assert(longest != anims.end());

            if (longest->is_done() && foo.is_stopped()) {
                foo.start();
            }

            DrawCircleV({foo, HEIGHT-radius}, radius, RED);
            DrawCircleV({foo, radius}, radius, RED);

        }
        EndDrawing();
    }

    CloseWindow();

    return EXIT_SUCCESS;
}
