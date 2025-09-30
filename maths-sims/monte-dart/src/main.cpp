#include <iostream>
#include <random>
#include <raylib.h>

// Check if point is inside the circle centered in the screen
bool inCircle(uint16_t dart_x, uint16_t dart_y, int screen_width, int screen_height, int radius) {
    int dx = dart_x - screen_width / 2;
    int dy = dart_y - screen_height / 2;
    return (dx * dx + dy * dy < radius * radius);
}

int main() {
    const int screen_width = 400;
    const int screen_height = 400;

    uint32_t finished = 0;
    float pi {};

    uint32_t max_darts = 100000;  
    uint32_t countC = 0;
    uint32_t countR = 0;

    std::random_device rd{};
    std::mt19937 mt{rd()};

    std::uniform_int_distribution<uint16_t> ud_x(0, (uint16_t)(screen_width - 1));
    std::uniform_int_distribution<uint16_t> ud_y(0, (uint16_t)(screen_height - 1));

    // Smoother edges
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(screen_width, screen_height, "Monte Carlo Dart Simulation");
    SetTargetFPS(120);

    // Off-screen render target to keep drawing darts
    RenderTexture2D dart_layer = LoadRenderTexture(screen_width, screen_height);
    
    BeginTextureMode(dart_layer);
    ClearBackground(BLANK);
    EndTextureMode();

    // Precompute board params
    int radius = screen_width / 2;
    Rectangle board = { 0, 0, (float)screen_width, (float)screen_height };

    while (!WindowShouldClose()) {
        // Draw several darts per frame for a snappier fill without changing overall logic
        int darts_this_frame = (finished < max_darts ? 200 : 0);

        for (int i = 0; i < darts_this_frame && finished < max_darts; ++i) {
            uint16_t dart_x = ud_x(mt);
            uint16_t dart_y = ud_y(mt);

            bool check = inCircle(dart_x, dart_y, screen_width, screen_height, radius);

            Color color = check ? Fade(GREEN, 0.85f) : Fade(RED, 0.85f);

            // Draw dart to the render texture (persistent)
            BeginTextureMode(dart_layer);
            // Slightly larger than a single pixel so dots are visible
            DrawRectangle(dart_x, dart_y, 2, 2, color);
            EndTextureMode();

            if (check) countC++;
            countR++;
            pi = static_cast<float>(countC) / countR * 4.0f;
            finished++;
        }


        BeginDrawing();
        ClearBackground(Color{ 24, 20, 37, 255 }); 

        // Board: subtle square fill + circle fill, then crisp outlines
        DrawRectangleRec(board, Color{ 30, 55, 153, 30 });                  
        DrawCircle(screen_width / 2, screen_height / 2, (float)radius, Color{ 255, 161, 0, 40 });
        DrawRectangleLinesEx(board, 2.0f, Color{ 90, 120, 255, 180 });     
        DrawCircleLines(screen_width / 2, screen_height / 2, (float)radius, Color{ 255, 161, 0, 220 }); 

        // Draw persistent dart layer (flip vertically)
        DrawTextureRec(dart_layer.texture, { 0, 0, (float)screen_width, -(float)screen_height }, { 0, 0 }, WHITE);

        // HUD
        int margin = 8;
        int line = 0;
        DrawText(TextFormat("π ≈ %.6f", pi), margin, margin + 22 * line++, 20, WHITE);
        DrawText(TextFormat("circle: %d   total: %d", countC, countR), margin, margin + 22 * line++, 18, Color{ 220, 220, 220, 255 });
        DrawText(TextFormat("error: %.6f", fabsf(pi - PI)), margin, margin + 22 * line++, 18, Color{ 200, 200, 200, 255 });

        // Progress bar
        float progress = (max_darts > 0) ? (float)finished / (float)max_darts : 1.0f;
        int bar_w = screen_width - margin * 2;
        int bar_h = 14;
        int bar_x = margin;
        int bar_y = screen_height - margin - bar_h;
        DrawRectangle(bar_x, bar_y, bar_w, bar_h, Color{ 60, 60, 90, 180 });
        DrawRectangle(bar_x, bar_y, (int)(bar_w * progress), bar_h, Color{ 80, 200, 120, 220 });
        DrawRectangleLines(bar_x, bar_y, bar_w, bar_h, Color{ 230, 230, 230, 200 });

        EndDrawing();
    }

    // Cleanup
    UnloadRenderTexture(dart_layer);
    CloseWindow();
    return 0;
}
