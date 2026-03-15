#include <cstdio>
#include <raylib.h>

namespace settings {

const int DefaultScreenWidth = 800;
const int DefaultScreenHeight = 800;
const Color DefaultBackgroundColor { 0x00, 0x00, 0x00, 0x00 };

}; // namespace settings

int main(void)
{
    SetTraceLogLevel(LOG_INFO);

    // SetConfigFlags(FLAG_MSAA_4X_HINT);

    InitWindow(settings::DefaultScreenWidth, settings::DefaultScreenHeight, "EmbeddedWatchViewer");

    SetTargetFPS(60);

    // static const Font DefaultFont = LoadFont("C:/Windows/Fonts/calibri.ttf");
    // static const float DefaultFontSize = 18.0f;

    // static const DrawOptions draw_options = DrawOptions { .font = DefaultFont, .font_size = DefaultFontSize };

    // Matrix01 matrix01(20, 20);

    // static int count = Settings::DefaultTimeout;

    int count = 0;

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_S)) {
            // matrix01.start();
        } else if (IsKeyPressed(KEY_N)) {
            // matrix01.next();
        }

        // if (count == 0) {
        //     matrix01.next();
        //     count = Settings::DefaultTimeout;
        // }

        // count--;

        BeginDrawing();

        ClearBackground(settings::DefaultBackgroundColor);

        // matrix01.draw(draw_options);
        char text[250];

        snprintf(text, sizeof(text), "Time: %d", count);

        DrawText(text, 50, 50, 32, WHITE);
        // DrawTextEx(options.font, message, Vector2 { text_x, text_y }, font_size, 1, WHITE);

        EndDrawing();

        count += 1;
    }

    CloseWindow();

    return 0;
}
