#include <iostream>
#include <vector>
#include "ball.hpp"

int main() {
    std::cout << "Starting the game" << std::endl;

    const int screen_width = 800;
    const int screen_height = 800;

    InitWindow(screen_width, screen_height, "Bouncing Ball Simulation");
    SetTargetFPS(120);

    // Create multiple balls with different directions and speeds
    std::vector<Ball> balls = {
        Ball(15.0f, {100.0f, 100.0f}, {100.0f, 150.0f}, RED),
        Ball(20.0f, {300.0f, 200.0f}, {-80.0f, 120.0f}, BLUE),
        Ball(10.0f, {500.0f, 300.0f}, {60.0f, -100.0f}, GREEN),
        Ball(25.0f, {200.0f, 600.0f}, {150.0f, -80.0f}, YELLOW),
        Ball(18.0f, {700.0f, 400.0f}, {-120.0f, 90.0f}, ORANGE)
    };

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        BeginDrawing();
        ClearBackground(DARKPURPLE);

        DrawText("Click on a ball to drag it. Release to throw.", 10, 10, 20, LIGHTGRAY);

        for (auto& ball : balls) {
            ball.HandleInput(GetMousePosition());
            ball.Update(dt);
            ball.CheckCollision(screen_width, screen_height);
            ball.Draw();
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
