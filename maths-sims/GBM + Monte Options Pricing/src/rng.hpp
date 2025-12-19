#pragma once
#include <random>

struct RNG {
    std::mt19937_64 eng;
    std::normal_distribution<double> norm{0.0, 1.0};

    explicit RNG(uint64_t seed = 0xC0FFEEULL) : eng(seed) {}

    double Z() { return norm(eng); }
};
