#include <print>
#include <array>
#include <ranges>

#include <raylib.h>
#include <raymath.h>

#define ANIM_INTEGRATION_RAYLIB
#include "anim.hh"





static constexpr int WIDTH = 1600;
static constexpr int HEIGHT = 900;


static void draw_text_centered(Vector2 center, char const* text, float fontsize) {
    float textsize = MeasureText(text, fontsize);
    DrawText(text, center.x-textsize/2, center.y-fontsize/2, fontsize, WHITE);
}

static void draw_text_centered(Vector2 center, Font font, char const* text, float fontsize) {
    float textsize = MeasureText(text, fontsize);
    Vector2 pos = { center.x-textsize/2, center.y-fontsize/2 };
    DrawTextEx(font, text, pos, fontsize, 0, WHITE);
}

class LoadingBarAnimation : public anim::AnimationTemplate {
    const Vector2 m_center;
    const float m_width;
    const float m_thickness;
    const float m_height;

    const Color m_color_bar;
    const Color m_color_outline;
    const Color m_color_end;

    Font m_font = LoadFont("/usr/share/fonts/TTF/Roboto-Regular.ttf");

    anim::Animation<Vector2> m_pos = init_pos();
    anim::Animation<float> m_bar_width { { 0, m_width, 2, anim::interpolators::ease_in_out_quint } };
    anim::Animation<float> m_roundness { { 1.0f, 0.0f, 1, anim::interpolators::ease_in_out_circ } };
    anim::Animation<Rectangle> m_rect = init_rect();
    anim::Animation<Color> m_anim_color_end = init_anim_color_end();

    anim::Batch m_box { m_roundness, m_rect };

public:
    LoadingBarAnimation(Vector2 center, float width, float thickness, float height,
                        Color color_bar, Color color_outline, Color color_end)
        : anim::AnimationTemplate({ m_pos, m_bar_width, m_box, m_anim_color_end })
        , m_center(center)
        , m_width(width)
        , m_thickness(thickness)
        , m_height(height)
        , m_color_bar(color_bar)
        , m_color_outline(color_outline)
        , m_color_end(color_end)
    { };

    void on_update() override {

        Vector2 start { m_pos->x - m_width/2, m_pos->y-m_height/2 };

        if (m_bar_width.is_running() || m_pos.is_running()) {
            draw_inner_bar(start);
            Rectangle rect = { start.x, start.y, m_width, m_height };
            DrawRectangleRoundedLinesEx(rect, 1, 0, m_thickness, m_color_outline);

            draw_percentage();
        }

        if (m_box.is_running() || m_anim_color_end.is_running()) {
            DrawRectangleRounded(m_rect, m_roundness, 0, m_anim_color_end);
        }

    }

private:
    void draw_percentage() const {
        double perc = m_pos.is_running() ? 0 : std::trunc(m_bar_width.get_progress()*100);
        auto text = std::format("{}%", perc);
        draw_text_centered(m_pos, m_font, text.c_str(), 50);
    }

    void draw_inner_bar(Vector2 start) const {

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

    [[nodiscard]] constexpr anim::Animation<Vector2> init_pos() const {
        Vector2 start = { WIDTH/2.0f, -m_height/2.0f-m_thickness };
        Vector2 end = { WIDTH/2.0f, HEIGHT/2.0f };
        return anim::Interpolator<Vector2> { start, end, 1.0f, anim::interpolators::ease_out_back };
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
        return anim::Interpolator<Color> { m_color_bar, m_color_end, 1, anim::interpolators::linear };
    }

};

class RotatingSquareAnimation : public anim::AnimationTemplate {
    float m_square_size = 300;

    anim::Animation<Vector2> m_pos = init_pos();
    anim::Animation<float> m_rotation = init_rotation();
    anim::Batch m_batch { m_pos, m_rotation };

public:
    RotatingSquareAnimation() : anim::AnimationTemplate(m_batch) { }

    void on_update() override {
        Rectangle rect = {
            m_pos->x,
            m_pos->y,
            m_square_size,
            m_square_size,
        };
        DrawRectanglePro(rect, { rect.width/2, rect.height/2 }, m_rotation, BLUE);
    }

private:
    [[nodiscard]] constexpr anim::Animation<float> init_rotation() {
        return {
            anim::Interpolator<float>::wait(0, 1),
            { 0, 180, 2, anim::interpolators::ease_in_out_back },
            anim::Interpolator<float>::wait(0, 1),
        };
    }

    [[nodiscard]] constexpr anim::Animation<Vector2> init_pos() {
        float y = HEIGHT/2.0f;
        Vector2 middle = {WIDTH/2.0f, y};

        auto fn = anim::interpolators::ease_in_out_expo;
        return {
            { {-m_square_size/2, y}, middle, 1, fn },
            anim::Interpolator<Vector2>::wait(middle, 2),
            { middle, {WIDTH+m_square_size/2, y}, 1, fn },
        };
    }

};


class SquareCircleLineAnimation : public anim::AnimationTemplate {
    float m_square_size = 50;
    float m_end = WIDTH/2.0f;
    float m_radius = m_square_size/2;
    int m_middle_spacing = 20;

    anim::Animation<float> square1 { { 0, m_end-m_radius-m_middle_spacing, 0.5f, anim::interpolators::ease_in_out_cubic } };
    anim::Animation<float> square2 { { 0, m_end-m_radius-m_middle_spacing, 1.0f, anim::interpolators::ease_in_out_cubic } };
    anim::Animation<float> square3 { { 0, m_end-m_radius-m_middle_spacing, 1.5f, anim::interpolators::ease_in_out_cubic } };

    anim::Animation<float> circle1 { { static_cast<float>(WIDTH-m_radius), m_end+m_radius+m_middle_spacing, 0.5f, anim::interpolators::ease_in_out_cubic } };
    anim::Animation<float> circle2 { { static_cast<float>(WIDTH-m_radius), m_end+m_radius+m_middle_spacing, 1.0f, anim::interpolators::ease_in_out_cubic } };
    anim::Animation<float> circle3 { { static_cast<float>(WIDTH-m_radius), m_end+m_radius+m_middle_spacing, 1.5f, anim::interpolators::ease_in_out_cubic } };

    anim::Animation<float> line1 {
        { HEIGHT/2.0f, HEIGHT, 0.5f, anim::interpolators::ease_in_quad },
        { HEIGHT, HEIGHT/2.0f, 0.5f, anim::interpolators::ease_in_quad },
    };

    anim::Animation<float> line2 {
        { HEIGHT/2.0f, 0, 0.5f, anim::interpolators::ease_in_quad },
        { 0, HEIGHT/2.0f, 0.5f, anim::interpolators::ease_in_quad },
    };

    anim::Batch m_squares { square1, square2, square3 };
    anim::Batch m_circles { circle1, circle2, circle3 };
    anim::Batch m_lines { line1, line2 };

public:
    SquareCircleLineAnimation() : anim::AnimationTemplate({ m_squares, m_circles, m_lines }) { }

    void on_update() override {

        if (m_anim.is_running()) {
            DrawRectanglePro({ square1, HEIGHT/2.0f-m_radius*3, m_square_size, m_square_size }, { m_radius, m_radius }, 0.0f, BLUE);
            DrawRectanglePro({ square2, HEIGHT/2.0f, m_square_size, m_square_size }, { m_radius, m_radius }, 0.0f, BLUE);
            DrawRectanglePro({ square3, HEIGHT/2.0f+m_radius*3, m_square_size, m_square_size }, { m_radius, m_radius }, 0.0f, BLUE);

            DrawCircle(circle1, HEIGHT/2.0f-m_radius*3, m_radius, RED);
            DrawCircle(circle2, HEIGHT/2.0f, m_radius, RED);
            DrawCircle(circle3, HEIGHT/2.0f+m_radius*3, m_radius, RED);
        }

        DrawLineEx({ m_end, line1 }, { m_end, line2 }, m_middle_spacing, PURPLE);
    }

};


class BouncingCirclesAnimation : public anim::AnimationTemplate {
    const int m_count;
    const float m_radius;
    const float m_offset = 100;
    const float m_start = HEIGHT/2.0f - m_offset;
    const float m_end = HEIGHT/2.0f + m_offset;

    std::vector<anim::Animation<float>> m_circles;
    anim::Batch m_batch;

public:
    BouncingCirclesAnimation(int count, int radius)
        : anim::AnimationTemplate(m_batch)
        , m_count(count)
        , m_radius(radius)
    {
        assert(m_count % 2 != 0 && "count must be odd");

        for (double i=1; i <= m_count; ++i) {

            double duration = i < m_count/2.0f ? i : m_count-i+1;
            auto &anim = m_circles.emplace_back(anim::Animation<float> {
                { m_start, m_end, duration/2.0f, anim::interpolators::ease_in_out_back },
            });

            anim.start();
        }

        for (auto& anim : m_circles)
            m_batch.add(anim);

        m_batch.start();
    }

    void on_update() override {

        if (!m_anim.is_running()) return;

        float spacing = 100;
        auto width = m_count * spacing - spacing;
        auto offset = WIDTH/2.0f - width/2.0f;

        std::size_t idx = 0; // std::views::enumerate() not supported by emscripten em++ :(
        for (auto &anim : m_circles) {
            float x0 = offset + spacing * idx;

            DrawLineEx({x0, m_start}, {x0, m_end}, 5, GRAY);
            DrawCircleV({x0, anim}, m_radius, BLUE);
            idx++;

        }
    }

};

int main() {

    InitWindow(WIDTH, HEIGHT, "animations");
    SetTargetFPS(180);

    LoadingBarAnimation loading_bar({ WIDTH/2.0f, HEIGHT/2.0f }, 500, 20, 100, DARKBLUE, BLUE, BLACK);
    RotatingSquareAnimation rot;
    SquareCircleLineAnimation scl;
    BouncingCirclesAnimation bc(9, 25);

    anim::Sequence seq { loading_bar, bc, scl, rot };
    seq.start();

    while (!WindowShouldClose()) {
        BeginDrawing();

        ClearBackground(BLACK);

        seq.dispatch();

        loading_bar.update();
        rot.update();
        scl.update();
        bc.update();

        if (seq.is_done())
            seq.start();

        EndDrawing();
    }

    CloseWindow();

    return EXIT_SUCCESS;
}
