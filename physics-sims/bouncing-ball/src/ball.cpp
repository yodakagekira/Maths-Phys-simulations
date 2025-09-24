#include "ball.hpp"
#include <iostream>

const float g = 981.0f;

Ball::Ball() 
    : radius(10.0f), position({400.0f, 400.0f}), velocity({0.0f, 0.0f}), color(RAYWHITE) 
{}

Ball::Ball(float r, Vector2 pos, Vector2 vel, Color col) 
    : radius(r), position(pos), velocity(vel), color(col) 
{}

void Ball::Update(float dt) {
    if(!isDragged) {
        velocity.y += (g * dt); // Simulate gravity effect
        position = Vector2Add(position, Vector2Scale(velocity, dt));
    }
}

void Ball::Draw() {
    DrawCircleV(position, radius, color);
}

void Ball::CheckCollision(int screenWidth, int screenHeight) {
    if(position.x - radius <= 0 || position.x + radius >= screenWidth) {
        velocity.x *= -1 * restitution;
    }
    if(position.y - radius <= 0 || position.y + radius >= screenHeight) {
        velocity.y *= -1;
    }

}   

void Ball::ApplyForce(Vector2 force) {
    velocity = Vector2Add(velocity, force);
}

void Ball::HandleInput(Vector2 mousePos) {
    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
        if (Vector2Distance(mousePos, position) <= radius) {
            isDragged = true;
        }
    } else {
        isDragged = false;
    }

    if (isDragged) {
        // Clamp position to window bounds while dragging
        int screenWidth = GetScreenWidth();
        int screenHeight = GetScreenHeight();

        // Prevent dragging outside boundaries
        position.x = Clamp(mousePos.x, radius, screenWidth - radius);
        position.y = Clamp(mousePos.y, radius, screenHeight - radius);
    }
}


// Setters
void Ball::SetRadius(float r) {
    radius = r;
}
void Ball::SetPosition(Vector2 pos) {
    position = pos;
}
void Ball::SetVelocity(Vector2 vel) {
    velocity = vel;
}
void Ball::SetColor(Color col) {
    color = col;
}


// Getters
float Ball::GetRadius() const {
    return radius;
}

Vector2 Ball::GetPosition() const {
    return position;
}
Vector2 Ball::GetVelocity() const {
    return velocity;
}
Color Ball::GetColor() const {
    return color;
}
