#include "particle.hpp"
#include <raymath.h>
#include <algorithm>

// ---- helpers ----
static inline Vector3 ClampSpeed(Vector3 v, float minSpeed, float maxSpeed) {
    float m = Vector3Length(v);
    if (m <= 1e-8f) return v;

    if (m > maxSpeed) return Vector3Scale(v, maxSpeed / m);
    if (m < minSpeed) return Vector3Scale(v, minSpeed / m);
    return v;
}

static inline bool InBox(const Vector3& p, const BoundingBox& b) {
    return (p.x >= b.min.x && p.x <= b.max.x &&
            p.y >= b.min.y && p.y <= b.max.y &&
            p.z >= b.min.z && p.z <= b.max.z);
}

static inline float Frand01() {
    return (float)GetRandomValue(0, 1000000) / 1000000.0f;
}

static inline Vector3 RandomPointInBox(const BoundingBox& b) {
    return {
        Lerp(b.min.x, b.max.x, Frand01()),
        Lerp(b.min.y, b.max.y, Frand01()),
        Lerp(b.min.z, b.max.z, Frand01())
    };
}

static inline void WrapIntoBox(Vector3& p, const BoundingBox& b) {
    const float sx = b.max.x - b.min.x;
    const float sy = b.max.y - b.min.y;
    const float sz = b.max.z - b.min.z;

    if (p.x < b.min.x) p.x += sx;
    if (p.x > b.max.x) p.x -= sx;

    if (p.y < b.min.y) p.y += sy;
    if (p.y > b.max.y) p.y -= sy;

    if (p.z < b.min.z) p.z += sz;
    if (p.z > b.max.z) p.z -= sz;
}

static inline void BounceOnBox(Vector3& p, Vector3& v, const BoundingBox& b) {
    if (p.x < b.min.x) { p.x = b.min.x; v.x = -v.x; }
    if (p.x > b.max.x) { p.x = b.max.x; v.x = -v.x; }

    if (p.y < b.min.y) { p.y = b.min.y; v.y = -v.y; }
    if (p.y > b.max.y) { p.y = b.max.y; v.y = -v.y; }

    if (p.z < b.min.z) { p.z = b.min.z; v.z = -v.z; }
    if (p.z > b.max.z) { p.z = b.max.z; v.z = -v.z; }
}

// ---- Particle3D ----
Particle3D::Particle3D(Vector3 p, Vector3 v) : pos(p), vel(v) {
    resetTrail();
}

void Particle3D::resetTrail() {
    trail.clear();
    if (!trailEnabled || trailLen <= 0) return;
    trail.reserve((size_t)trailLen);
    for (int i = 0; i < trailLen; ++i) trail.push_back(pos);
}

void Particle3D::applyForce(Vector3 f) {
    // F = m a  =>  a += F/m
    if (mass <= 1e-8f) return;
    acc = Vector3Add(acc, Vector3Scale(f, 1.0f / mass));
}


void Particle3D::step(WindFn windFn, float t, float dt, BoundingBox box, BoundsMode mode) {
    // Apply wind force
    Vector3 wind = windFn(pos, t);
    applyForce(wind);

    // Integrate acceleration
    vel = Vector3Add(vel, Vector3Scale(acc, dt));
    acc = {0,0,0}; // reset acceleration

    // Apply drag
    vel = Vector3Scale(vel, 1.0f - drag);

    // Clamp speed
    vel = ClampSpeed(vel, minSpeed, maxSpeed);

    // Integrate velocity
    pos = Vector3Add(pos, Vector3Scale(vel, dt));

    // Handle bounds
    bool inBox = InBox(pos, box);
    if (!inBox) {
        switch (mode) {
            case BoundsMode::Wrap:
                WrapIntoBox(pos, box);
                break;
            case BoundsMode::Bounce:
                BounceOnBox(pos, vel, box);
                break;
            case BoundsMode::Respawn:
                pos = RandomPointInBox(box);
                vel = {0,0,0};
                break;
        }
    }

    // Update trail
    if (trailEnabled && trailLen > 0) {
        trail.push_back(pos);
        if (trail.size() > (size_t)trailLen) {
            trail.erase(trail.begin());
        }
    }
}

void Particle3D::draw(float radius) const {
    DrawSphere(pos, radius, RAYWHITE);
}

void Particle3D::drawTrail() const {
    if (!trailEnabled || trail.size() < 2) return;
    for (size_t i = 1; i < trail.size(); ++i) {
        DrawLine3D(trail[i - 1], trail[i], RAYWHITE);
    }
}
