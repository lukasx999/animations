#include <print>
#include <array>
#include <ranges>

#include <raylib.h>
#include <raymath.h>

#define ANIM_INTEGRATION_RAYLIB
#include "anim.hh"





#define PRINT(x) std::println("{}: {}", #x, x);

static constexpr auto WIDTH = 1600;
static constexpr auto HEIGHT = 900;







static void draw_text_centered(Vector2 center, char const* text, float fontsize) {
    float textsize = MeasureText(text, fontsize);
    DrawText(text, center.x-textsize/2.0f, center.y-fontsize/2, fontsize, WHITE);
}

class LoadingBarAnimation : public anim::AnimationTemplate {
    const Vector2 m_center;
    const float m_width;
    const float m_thickness;
    const float m_height;

    const Color m_color_bar;
    const Color m_color_outline;
    const Color m_color_end;

    anim::Animation<float> m_bar_width { { 0, m_width, 1, anim::interpolators::ease_in_out_circ } };
    anim::Animation<float> m_roundness { { 1.0f, 0.0f, 1, anim::interpolators::ease_in_expo } };
    anim::Animation<Rectangle> m_rect = init_rect();
    anim::Animation<Color> m_anim_color_end = init_anim_color_end();

    anim::Batch m_box { m_roundness, m_rect };

public:
    LoadingBarAnimation(Vector2 center, float width, float thickness, float height,
                        Color color_bar, Color color_outline, Color color_end)
        : anim::AnimationTemplate({ m_bar_width, m_box, m_anim_color_end })
        , m_center(center)
        , m_width(width)
        , m_thickness(thickness)
        , m_height(height)
        , m_color_bar(color_bar)
        , m_color_outline(color_outline)
        , m_color_end(color_end)
    { };

    void draw() {

        m_anim.dispatch();

        Vector2 start { m_center.x - m_width/2.0f, m_center.y-m_height/2 };

        if (m_bar_width.is_running()) {
            draw_inner_bar();
            Rectangle rect = { start.x, start.y, m_width, m_height };
            DrawRectangleRoundedLinesEx(rect, 1, 0, m_thickness, m_color_outline);
        }

        if (m_box.is_running()) {
            DrawRectangleRounded(m_rect, m_roundness, 0, m_anim_color_end);
        }

    }

private:
    void draw_inner_bar() const {

        Vector2 start { m_center.x - m_width/2.0f, m_center.y-m_height/2 };
        float radius = m_height/2;

        BeginScissorMode(start.x, start.y, m_bar_width, m_height);
            DrawCircleV({ start.x+radius, start.y+radius }, radius, m_color_bar);
        EndScissorMode();

        if (m_bar_width > m_width-radius) {
            BeginScissorMode(start.x, start.y, m_bar_width, m_height);
                DrawCircleV({ start.x+m_width-radius, start.y+radius }, radius, m_color_bar);
            EndScissorMode();
        }

        if (m_bar_width >= radius) {
            BeginScissorMode(start.x+radius, start.y, m_width-radius*2, m_height);
                DrawRectangleRec({ start.x+radius, start.y, m_bar_width-radius, m_height }, m_color_bar);
            EndScissorMode();
        }
    }

    [[nodiscard]] constexpr anim::Animation<Rectangle> init_rect() const {

        Rectangle start = {
            m_center.x - m_width/2,
            m_center.y - m_height/2,
            m_width,
            m_height,
        };

        Rectangle end = { 0, 0, WIDTH, HEIGHT };

        return anim::Interpolator<Rectangle> { start, end, 1, anim::interpolators::ease_in_out_quint };
    }

    [[nodiscard]] constexpr anim::Animation<Color> init_anim_color_end() const {
        return anim::Interpolator<Color> { m_color_bar, m_color_end, 1, anim::interpolators::ease_in_out_quad };
    }

};

int main() {

    InitWindow(WIDTH, HEIGHT, "animations");
    SetTargetFPS(180);

    anim::Animation<float> a({ 0, 100, 2, anim::interpolators::linear });
    anim::Animation<float> b({ 0, 100, 2, anim::interpolators::linear });
    anim::Animation<float> c({ 0, 100, 2, anim::interpolators::linear });
    anim::Sequence seq { a, b, c };
    seq.start();

    while (!WindowShouldClose()) {
        BeginDrawing();
        {
            ClearBackground(BLACK);

            seq.dispatch();

        }
        EndDrawing();
    }

    CloseWindow();

    return EXIT_SUCCESS;
}

int main1() {

    InitWindow(WIDTH, HEIGHT, "animations");
    SetTargetFPS(180);

    LoadingBarAnimation loading_bar({ WIDTH/2.0f, HEIGHT/2.0f }, 500, 10, 100, MAROON, ORANGE, BLACK);

    anim::Animation<float> rot({ 0, 180, 2, anim::interpolators::ease_in_out_quad });
    anim::Sequence seq { rot, loading_bar };

    seq.start();

    while (!WindowShouldClose()) {
        BeginDrawing();
        {
            ClearBackground(BLACK);

            seq.dispatch();

            loading_bar.draw();

            if (rot.is_running()) {
                Rectangle rect = { WIDTH/2.0f, HEIGHT/2.0f, 300, 300};
                DrawRectanglePro(rect, { rect.width/2, rect.height/2 }, rot, BLUE);
            }

        }
        EndDrawing();
    }

    CloseWindow();

    return EXIT_SUCCESS;
}

int main__() {

    InitWindow(WIDTH, HEIGHT, "animations");
    SetTargetFPS(180);

    int sq_size = 50;

    float end = WIDTH/2.0f - sq_size;
    int rad = sq_size/2;
    int spacing = 20;

    anim::Animation<float> square1({ 0, end-spacing, 1.0f, anim::interpolators::ease_in_out_cubic });
    anim::Animation<float> square2({ 0, end-spacing, 1.0f, anim::interpolators::ease_in_out_cubic });
    anim::Animation<float> square3({ 0, end-spacing, 1.0f, anim::interpolators::ease_in_out_cubic });

    anim::Animation<float> circle1({ static_cast<float>(WIDTH-rad), end+sq_size+rad+spacing, 1.0f, anim::interpolators::ease_in_out_cubic });
    anim::Animation<float> circle2({ static_cast<float>(WIDTH-rad), end+sq_size+rad+spacing, 1.0f, anim::interpolators::ease_in_out_cubic });
    anim::Animation<float> circle3({ static_cast<float>(WIDTH-rad), end+sq_size+rad+spacing, 1.0f, anim::interpolators::ease_in_out_cubic });

    anim::Animation<float> line1 {
        { HEIGHT/2.0f, HEIGHT, 1.0f, anim::interpolators::ease_in_quad },
        { HEIGHT, HEIGHT/2.0f, 1.0f, anim::interpolators::ease_in_quad },
    };

    anim::Animation<float> line2 {
        { HEIGHT/2.0f, 0, 1.0f, anim::interpolators::ease_in_quad },
        { 0, HEIGHT/2.0f, 1.0f, anim::interpolators::ease_in_quad },
    };

    anim::Batch squares { square1, square2, square3 };
    anim::Batch circles { circle1, circle2, circle3 };
    anim::Batch lines { line1, line2 };

    anim::Sequence seq { squares, circles, lines };
    seq.start();


    while (!WindowShouldClose()) {
        BeginDrawing();
        {
            ClearBackground(BLACK);

            seq.dispatch();

            DrawRectangle(square1, HEIGHT/2.0f-sq_size*2, sq_size, sq_size, BLUE);
            DrawRectangle(square2, HEIGHT/2.0f, sq_size, sq_size, BLUE);
            DrawRectangle(square3, HEIGHT/2.0f+sq_size*2, sq_size, sq_size, BLUE);

            DrawCircle(circle1, HEIGHT/2.0f-sq_size*2+rad, rad, RED);
            DrawCircle(circle2, HEIGHT/2.0f+rad, rad, RED);
            DrawCircle(circle3, HEIGHT/2.0f+sq_size*2+rad, rad, RED);

            DrawLineEx({ end+sq_size, line1 }, { end+sq_size, line2 }, spacing, PURPLE);

        }
        EndDrawing();
    }

    CloseWindow();

    return EXIT_SUCCESS;
}

int main_() {

    InitWindow(WIDTH, HEIGHT, "animations");
    SetTargetFPS(180);

    float radius = 50;
    float offset = 500;

    anim::Animation<float> rad {
        { 0, radius, 1, anim::interpolators::ease_in_quad },
        { radius, 0, 1, anim::interpolators::ease_in_quad },
    };

    anim::Animation<Color> col {
        { RED, BLUE, 2, anim::interpolators::ease_in_cubic },
    };

    anim::Animation<Vector2> vect {
        { Vector2 { radius, radius }, Vector2 { WIDTH-radius-offset, radius }, 1, anim::interpolators::ease_in_out_cubic },
        { Vector2 { WIDTH-radius-offset, radius }, Vector2 { WIDTH-radius-offset, HEIGHT-radius-offset }, 1, anim::interpolators::ease_in_out_cubic },
    };

    anim::Animation<float> pos {
        { radius, WIDTH-radius, 1, anim::interpolators::ease_in_out_cubic },
    };

    anim::Batch batch { rad, col, vect };
    anim::Sequence seq { batch, pos };
    seq.start();

    while (!WindowShouldClose()) {
        BeginDrawing();
        {
            ClearBackground(BLACK);
            seq.dispatch();

            if (IsKeyPressed(KEY_J))
                seq.start();

            DrawCircle(pos, HEIGHT/4.0f, radius, ORANGE);
            DrawCircleV(vect, rad, col);

        }
        EndDrawing();
    }

    CloseWindow();

    return EXIT_SUCCESS;
}

int main____() {

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

    anim::Batch batch;

    for (auto& anim : anims)
        batch.add(anim);

    batch.start();

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

        }
        EndDrawing();
    }

    CloseWindow();

    return EXIT_SUCCESS;
}
