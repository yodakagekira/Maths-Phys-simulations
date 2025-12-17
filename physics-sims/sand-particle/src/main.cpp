#include "sand.hpp"

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT,
               "Advanced Falling Sand - raylib");
    SetTargetFPS(60);

    for (int x = 0; x < GRID_WIDTH; x++)
        for (int y = 0; y < GRID_HEIGHT; y++)
            grid[x][y] = AIR;

    Image screenImage = GenImageColor(SCREEN_WIDTH, SCREEN_HEIGHT, SKYBLUE);
    Texture2D screenTexture = LoadTextureFromImage(screenImage);

    CellType currentMaterial = SAND;
    int brushSize = 6;
    bool paused = false;

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_ONE))   currentMaterial = SAND;
        if (IsKeyPressed(KEY_TWO))   currentMaterial = WATER;
        if (IsKeyPressed(KEY_THREE)) currentMaterial = STONE;
        if (IsKeyPressed(KEY_FOUR))  currentMaterial = OIL;
        if (IsKeyPressed(KEY_FIVE))  currentMaterial = FIRE;
        if (IsKeyPressed(KEY_SPACE)) paused = !paused;

        if (IsKeyDown(KEY_LEFT))  brushSize = (brushSize > 1)  ? brushSize - 1 : 1;
        if (IsKeyDown(KEY_RIGHT)) brushSize = (brushSize < 20) ? brushSize + 1 : 20;

        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) ||
            IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) {

            Vector2 mouse = GetMousePosition();
            int gx = mouse.x / CELL_SIZE;
            int gy = mouse.y / CELL_SIZE;

            CellType type =
                IsMouseButtonDown(MOUSE_LEFT_BUTTON) ? currentMaterial : AIR;

            for (int dx = -brushSize; dx <= brushSize; dx++) {
                for (int dy = -brushSize; dy <= brushSize; dy++) {
                    int nx = gx + dx, ny = gy + dy;
                    if (nx >= 0 && nx < GRID_WIDTH &&
                        ny >= 0 && ny < GRID_HEIGHT &&
                        dx*dx + dy*dy <= brushSize*brushSize) {
                        grid[nx][ny] = type;
                        if (type == FIRE) fireLife[nx][ny] = 40;
                    }
                }
            }
        }

        if (!paused) UpdateSimulation();

        DrawGrid(screenImage);
        UpdateTexture(screenTexture, screenImage.data);

        BeginDrawing();
        ClearBackground(SKYBLUE);
        DrawTexture(screenTexture, 0, 0, WHITE);
        DrawFPS(10, 10);
        EndDrawing();
    }

    UnloadTexture(screenTexture);
    UnloadImage(screenImage);
    CloseWindow();
    return 0;
}
