#pragma once
#include <raylib.h>
#include <raymath.h>
#include <algorithm>
#include <vector>

using WindFn = Vector3 (*)(Vector3, float);


enum class BoundsMode {
    Wrap,
    Bounce,
    Respawn
};

struct Particle3D {
    Vector3 pos{0,0,0};
    Vector3 vel{0,0,0};
    Vector3 acc{0,0,0};

    float minSpeed = 0.0f;
    float maxSpeed = 6.0f;
    float drag     = 0.05f;   // 0..1 (higher = more damping)
    float mass     = 1.0f;

    // Visuals (optional trail)
    bool  trailEnabled = true;
    int   trailLen     = 32;
    std::vector<Vector3> trail;

    Particle3D() = default;
    Particle3D(Vector3 p, Vector3 v);

    void resetTrail();
    void applyForce(Vector3 f);


void step(WindFn windFn, float t, float dt, BoundingBox box,
          BoundsMode mode = BoundsMode::Wrap);


    void draw(float radius = 0.03f) const;         // particle as small sphere
    void drawTrail() const;                        // line trail
};

