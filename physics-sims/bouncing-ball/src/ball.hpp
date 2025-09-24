#pragma once

#include <raylib.h>
#include <raymath.h>
#include <vector>
#include <cmath>

class Ball {
private:
    float radius;
    Vector2 position;
    Vector2 velocity;
    bool isDragged = false;
    Color color;
    float restitution = 0.970f; // Coefficient of restitution (bounciness)

public:
    Ball();  // Default constructor
    Ball(float r, Vector2 pos, Vector2 vel, Color col);  // Custom constructor

    void Update(float dt);
    void Draw();
    
    void CheckCollision(int screenWidth, int screenHeight);
    void ApplyForce(Vector2 force);
    void HandleInput(Vector2 mousePos);

    // Setters
    void SetRadius(float r);
    void SetPosition(Vector2 pos);
    void SetVelocity(Vector2 vel);
    void SetColor(Color col);

    // Getters
    float GetRadius() const;
    Vector2 GetPosition() const;
    Vector2 GetVelocity() const;
    Color GetColor() const;
};
