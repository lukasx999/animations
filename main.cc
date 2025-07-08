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


class Animation {
    struct Keyframe {
        float diff;
        double duration;
        std::function<float(float)> interp_fn;
    };
    std::vector<Interpolator<float>> m_interpolators;
    std::vector<Interpolator<float>>::iterator m_it;
    double m_start_time = 0.0f;

public:
    Animation(std::initializer_list<Interpolator<float>> interps)
        : m_interpolators(interps)
        , m_it(m_interpolators.begin())
    { }

    Animation(float start, std::initializer_list<Keyframe> kfs) {
        float last = start;
        for (auto &kf : kfs) {
            m_interpolators.emplace_back(last, last+kf.diff, kf.duration, kf.interp_fn);
            last += kf.diff;
        }
    };

    void start() {
        m_start_time = get_time_secs();
        // m_it->start();
    }

    void reset() {
        // TODO:
    }

    float get() {

        double time = get_time_secs() - m_start_time;
        double acc = 0.0f;

        for (auto &interp : m_interpolators) {
            double duration = interp.m_duration;
            acc += interp.m_duration;

            if (time <= acc) {
                double diff = acc - time;
                return interp.get(duration - diff);
            }

        }

        return m_interpolators[m_interpolators.size()-1].m_end;
    }

    operator float() {
        return get();
    }

};

int main() {

    InitWindow(WIDTH, HEIGHT, "animations");
    SetTargetFPS(60);

    float radius = 50;

    Animation anim_x {
        0,
        {
            { 200, 2, easings::ease_out_expo },
            { 200, 2, easings::cubed },
            { 200, 2, easings::ease_in_out_cubic },
        }
    };
    anim_x.start();

    while (!WindowShouldClose()) {
        BeginDrawing();
        {
            ClearBackground(BLACK);

            if (!IsKeyDown(KEY_J)) {
                DrawCircleV({anim_x, HEIGHT/2.0f-radius/2.0f}, radius, BLUE);
            }

        }
        EndDrawing();
    }

    CloseWindow();

    return EXIT_SUCCESS;
}


// int main() {
//
//     InitWindow(WIDTH, HEIGHT, "animations");
//     SetTargetFPS(60);
//
//     float width = 100, height = width;
//     Interpolator<float> x(0, WIDTH-width*2, 1, easings::ease_in_out_cubic);
//     Interpolator<float> delta(0, 50, 0.5, easings::ease_out_expo);
//     Interpolator<float> delta_color(0, 1.0f, 0.3, easings::squared);
//     x.start();
//     bool start = false;
//
//     while (!WindowShouldClose()) {
//         BeginDrawing();
//         {
//             ClearBackground(BLACK);
//
//             float y = HEIGHT/2.0f - height/2.0f;
//
//             Rectangle rect { x, y, width, height };
//             DrawRectangleRec(rect, BLUE);
//
//             if (x.is_done()) {
//
//                 if (!start) {
//                     delta.start();
//                     delta_color.start();
//                     start = true;
//                 }
//
//                 Rectangle rect { x-delta, y-delta, width+delta*2, height+delta*2 };
//                 auto color = ColorLerp(RED, BLACK, delta_color);
//                 DrawRectangleRec(rect, color);
//             }
//
//         }
//         EndDrawing();
//     }
//
//     CloseWindow();
//
//     return EXIT_SUCCESS;
// }
