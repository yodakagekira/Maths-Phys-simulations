#include "perlin.hpp"

#include <array>
#include <algorithm>
#include <numeric>
#include <random>
#include <cmath>

namespace noise {

static std::array<int, 512> p{};
static bool initialized = false;

static inline float fade(float t) {
    return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
}

static inline float lerp(float a, float b, float t) {
    return a + t * (b - a);
}

// Gradient function: picks a gradient direction based on hash
static inline float grad(int hash, float x, float y, float z) {
    // Take the low 4 bits of the hash (0..15)
    const int h = hash & 15;

    // Use x,y,z combinations to create 12-ish directions
    const float u = (h < 8) ? x : y;
    const float v = (h < 4) ? y : ((h == 12 || h == 14) ? x : z);

    const float a = ((h & 1) == 0) ? u : -u;
    const float b = ((h & 2) == 0) ? v : -v;
    return a + b;
}

static void initDefault() {
    // Default permutation (0..255 shuffled) expanded to 512
    std::array<int, 256> perm{};
    std::iota(perm.begin(), perm.end(), 0);

    // Fixed seed by default so it’s deterministic without calling setSeed()
    std::mt19937 rng(1337u);
    std::shuffle(perm.begin(), perm.end(), rng);

    for (int i = 0; i < 256; ++i) {
        p[i] = perm[i];
        p[i + 256] = perm[i];
    }
    initialized = true;
}

float perlin3(float x, float y, float z) {
    if (!initialized) initDefault();

    // Find unit cube that contains the point
    const int X = static_cast<int>(std::floor(x)) & 255;
    const int Y = static_cast<int>(std::floor(y)) & 255;
    const int Z = static_cast<int>(std::floor(z)) & 255;

    // Find relative position in cube
    const float xf = x - std::floor(x);
    const float yf = y - std::floor(y);
    const float zf = z - std::floor(z);

    // Compute fade curves
    const float u = fade(xf);
    const float v = fade(yf);
    const float w = fade(zf);

    // Hash coordinates of the 8 cube corners
    const int A  = p[X] + Y;
    const int AA = p[A] + Z;
    const int AB = p[A + 1] + Z;

    const int B  = p[X + 1] + Y;
    const int BA = p[B] + Z;
    const int BB = p[B + 1] + Z;

    // Blend results from 8 corners
    const float x1 = lerp(
        grad(p[AA],     xf,     yf,     zf),
        grad(p[BA],     xf - 1, yf,     zf),
        u
    );
    const float x2 = lerp(
        grad(p[AB],     xf,     yf - 1, zf),
        grad(p[BB],     xf - 1, yf - 1, zf),
        u
    );
    const float y1 = lerp(x1, x2, v);

    const float x3 = lerp(
        grad(p[AA + 1], xf,     yf,     zf - 1),
        grad(p[BA + 1], xf - 1, yf,     zf - 1),
        u
    );
    const float x4 = lerp(
        grad(p[AB + 1], xf,     yf - 1, zf - 1),
        grad(p[BB + 1], xf - 1, yf - 1, zf - 1),
        u
    );
    const float y2 = lerp(x3, x4, v);

    // Final interpolation along z
    return lerp(y1, y2, w); // roughly [-1, 1]
}

float fbm3(float x, float y, float z, int octaves, float lacunarity, float gain) {
    float sum = 0.0f;
    float amp = 1.0f;
    float freq = 1.0f;

    // Normalization so output stays ~[-1,1]
    float norm = 0.0f;

    for (int i = 0; i < octaves; ++i) {
        sum += amp * perlin3(x * freq, y * freq, z * freq);
        norm += amp;
        freq *= lacunarity;
        amp *= gain;
    }

    return (norm > 0.0f) ? (sum / norm) : 0.0f;
}

} 
