// main.cpp
#include <raylib.h>
#include <raymath.h>

#include "particle.hpp"
#include "perlin.hpp"

#include <vector>
#include <algorithm>

/* build:
g++ src/perlin.cpp src/particle.cpp src/main.cpp -o main.exe -lraylib -lopengl32 -lgdi32 -lwinmm
*/

static inline Vector3 SafeNormalize(Vector3 v) {
    float m = Vector3Length(v);
    return (m < 1e-6f) ? (Vector3){0,0,0} : Vector3Scale(v, 1.0f / m);
}

static inline float Falloff(float r, float coreR, float outerR) {
    if (r <= coreR) return 1.0f;
    if (r >= outerR) return 0.0f;
    float t = (r - coreR) / (outerR - coreR); // 0..1
    float s = 1.0f - t;
    return s * s * (3.0f - 2.0f * s);         // smoothstep
}

// Tornado + weather: base vortex dominates, noise is just turbulence
static Vector3 SampleWind(Vector3 p, float t)
{
    // --- match your big box scale ---
    const Vector3 c = {0,0,0};
    const float coreR  = 12.0f;
    const float outerR = 90.0f;

    const float swirlS   = 9.0f;
    const float inflowS  = 4.0f;
    const float updraftS = 7.0f;
    const float shearS   = 1.6f;

    const float turbS = 0.45f;   // keep low so it doesn't look random
    const float ns = 0.03f;      // noise spatial scale for ~100-unit world
    const float nt = 0.25f;      // noise time speed

    Vector3 q = Vector3Subtract(p, c);
    float r = sqrtf(q.x*q.x + q.z*q.z);

    Vector3 radial = SafeNormalize((Vector3){q.x, 0.0f, q.z});
    Vector3 tang   = (Vector3){-radial.z, 0.0f, radial.x};

    float f = Falloff(r, coreR, outerR);

    // Core shaping so r->0 doesn't explode
    float coreShape = r / (r + coreR);

    // Slight altitude shaping (fits your y in [-50,50])
    float y = q.y;
    float inflowY = Clamp(1.0f - (y + 10.0f) / 80.0f, 0.25f, 1.0f);
    float upY     = Clamp(0.7f + (y + 10.0f) / 120.0f, 0.5f, 1.4f);

    Vector3 swirl   = Vector3Scale(tang,   swirlS   * f * (0.6f + 1.0f*coreShape));
    Vector3 inflow  = Vector3Scale(radial, -inflowS * f * inflowY);
    Vector3 updraft = Vector3Scale((Vector3){0,1,0}, updraftS * f * upY);

    // Background storm shear (wind changes with height)
    Vector3 shear = { shearS * (0.2f + 0.01f * y), 0.0f, 0.35f * shearS };

    // Turbulence (small perturbation)
    float tx = noise::fbm3(p.x*ns + 13.1f, p.y*ns + 7.7f,  p.z*ns + t*nt, 5, 2.0f, 0.5f);
    float ty = noise::fbm3(p.x*ns + 21.4f, p.y*ns + 19.2f, p.z*ns + t*nt, 5, 2.0f, 0.5f);
    float tz = noise::fbm3(p.x*ns + 5.6f,  p.y*ns + 31.8f, p.z*ns + t*nt, 5, 2.0f, 0.5f);

    Vector3 turb = SafeNormalize((Vector3){tx, 0.25f*ty, tz});
    turb = Vector3Scale(turb, turbS * f);

    Vector3 wind = {0,0,0};
    wind = Vector3Add(wind, swirl);
    wind = Vector3Add(wind, inflow);
    wind = Vector3Add(wind, updraft);
    wind = Vector3Add(wind, shear);
    wind = Vector3Add(wind, turb);
    return wind;
}

static Vector3 RandInBox(BoundingBox b) {
    auto fr = [](){ return (float)GetRandomValue(0,1000000) / 1000000.0f; };
    return (Vector3){
        Lerp(b.min.x, b.max.x, fr()),
        Lerp(b.min.y, b.max.y, fr()),
        Lerp(b.min.z, b.max.z, fr())
    };
}

int main() {
    InitWindow(1280, 720, "3D Tornado Flowfield");
    SetTargetFPS(120);

    Camera3D cam{};
    cam.position   = { 160.0f, 90.0f, 160.0f };
    cam.target     = { 0.0f, 0.0f, 0.0f };
    cam.up         = { 0.0f, 1.0f, 0.0f };
    cam.fovy       = 60.0f;
    cam.projection = CAMERA_PERSPECTIVE;

    BoundingBox box;
    box.min = { -100.0f, -50.0f, -100.0f };
    box.max = {  100.0f,  50.0f,  100.0f };

    const int N = 6000;
    std::vector<Particle3D> ps; ps.reserve(N);

    for (int i = 0; i < N; ++i) {
        Particle3D p;
        p.pos = RandInBox(box);
        p.vel = {0,0,0};

        p.minSpeed = 0.3f;
        p.maxSpeed = 9.0f;
        p.drag     = 0.03f;

        p.trailEnabled = true;
        p.trailLen = 28;
        p.resetTrail();

        ps.push_back(std::move(p));
    }

    bool drawTrails = true;
    BoundsMode mode = BoundsMode::Wrap;
    float simTime = 0.0f;

    while (!WindowShouldClose()) {
        float dt = std::min(GetFrameTime(), 1.0f/30.0f);
        simTime += dt;

        UpdateCamera(&cam, CAMERA_FREE);
        if (IsKeyPressed(KEY_T)) drawTrails = !drawTrails;
        if (IsKeyPressed(KEY_ONE)) mode = BoundsMode::Wrap;
        if (IsKeyPressed(KEY_TWO)) mode = BoundsMode::Bounce;
        if (IsKeyPressed(KEY_THREE)) mode = BoundsMode::Respawn;

        for (auto& p : ps) p.step(SampleWind, simTime, dt, box, mode);

        BeginDrawing();
        ClearBackground(BLACK);

        BeginMode3D(cam);
        DrawBoundingBox(box, DARKGRAY);
        DrawGrid(0, 0.0f);

        for (const auto& p : ps) {
            if (drawTrails) p.drawTrail();
            else p.draw(0.06f);
        }
        EndMode3D();

        DrawText("T: trails   1:wrap 2:bounce 3:respawn", 10, 10, 18, RAYWHITE);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
