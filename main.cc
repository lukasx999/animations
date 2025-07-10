#include <print>
#include <iostream>
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

    int sq_size = 50;

    float end = WIDTH/2.0f - sq_size;
    int rad = sq_size/2;
    int spacing = 20;

    anim::Batch<float> squares {
        { { 0, end-spacing, 1.0f, anim::interpolators::ease_in_out_cubic } },
        { { 0, end-spacing, 1.0f, anim::interpolators::ease_in_out_cubic } },
        { { 0, end-spacing, 1.0f, anim::interpolators::ease_in_out_cubic } },
    };

    anim::Batch<float> circles {
        { { static_cast<float>(WIDTH-rad), end+sq_size+rad+spacing, 1.0f, anim::interpolators::ease_in_out_cubic } },
        { { static_cast<float>(WIDTH-rad), end+sq_size+rad+spacing, 1.0f, anim::interpolators::ease_in_out_cubic } },
        { { static_cast<float>(WIDTH-rad), end+sq_size+rad+spacing, 1.0f, anim::interpolators::ease_in_out_cubic } },
    };

    anim::Batch<float> line {
        {
            { HEIGHT/2.0f, HEIGHT, 1.0f, anim::interpolators::squared },
            { HEIGHT, HEIGHT/2.0f, 1.0f, anim::interpolators::squared },
        },

        {
            { HEIGHT/2.0f, 0, 1.0f, anim::interpolators::squared },
            { 0, HEIGHT/2.0f, 1.0f, anim::interpolators::squared },
        },
    };

    // anim::Sequence<float> seq { squares, circles, line };

    squares.start();

    while (!WindowShouldClose()) {
        BeginDrawing();
        {
            ClearBackground(BLACK);

            DrawRectangle(squares[0], HEIGHT/2.0f-sq_size*2, sq_size, sq_size, BLUE);
            DrawRectangle(squares[1], HEIGHT/2.0f, sq_size, sq_size, BLUE);
            DrawRectangle(squares[2], HEIGHT/2.0f+sq_size*2, sq_size, sq_size, BLUE);

            DrawCircle(circles[0], HEIGHT/2.0f-sq_size*2+rad, rad, RED);
            DrawCircle(circles[1], HEIGHT/2.0f+rad, rad, RED);
            DrawCircle(circles[2], HEIGHT/2.0f+sq_size*2+rad, rad, RED);

            DrawLineEx({ end+sq_size, line[0] }, { end+sq_size, line[1] }, spacing, PURPLE);

            // if (squares.is_done() && circles.is_done() && line.is_stopped()) {
            //     line.start();
            // }

            // if (squares.is_done() && circles.is_stopped()) {
            //     circles.start();
            // }

            line.start_after({circles, squares});
            circles.start_after({squares});

        }
        EndDrawing();
    }

    CloseWindow();

    return EXIT_SUCCESS;
}

int main3() {

    InitWindow(WIDTH, HEIGHT, "animations");
    SetTargetFPS(180);

    float radius = 50;
    float offset = 500;

    anim::Animation<float> rad {
        { 0, radius, 1, anim::interpolators::squared },
        { radius, 0, 1, anim::interpolators::squared },
    };

    anim::Animation<Color> col {
        { RED, BLUE, 2, anim::interpolators::cubed },
    };

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

    constexpr int n = 9;
    static_assert(n % 2 != 0, "must be odd");

    bool flip = false;

    for (double i=1; i <= n; ++i) {

        double duration = i < n/2.0f ? i : n-i+1;
        auto &anim = anims.emplace_back(anim::Animation<float> {
            { start, end, duration/2.0f, anim::interpolators::ease_in_out_back },
        });

        anim.start();
    }

    while (!WindowShouldClose()) {
        BeginDrawing();
        {
            ClearBackground(BLACK);

            float spacing = 100;
            auto width = n * spacing - spacing;
            auto offset = WIDTH/2.0f - width/2.0f;

            for (auto &&[idx, anim] : std::views::enumerate(anims)) {
                float x0 = offset + spacing * idx;

                // DrawLineEx({x0, start}, {x0, end}, 5, GRAY);
                auto value = flip ? start+(end-anim) : anim;
                DrawCircleV({x0, value}, radius, BLUE);

            }

            auto longest = std::ranges::max_element(anims, [](const anim::Animation<float> &a, const anim::Animation<float> &b) {
                return b.get_duration() > a.get_duration();
            });
            assert(longest != anims.end());

            if (longest->is_done()) {
                flip = !flip;
                for (auto &anim : anims) anim.start();
            }

        }
        EndDrawing();
    }

    CloseWindow();

    return EXIT_SUCCESS;
}
