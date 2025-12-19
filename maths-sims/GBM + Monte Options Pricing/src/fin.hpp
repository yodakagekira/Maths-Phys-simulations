#pragma once
#include <vector>
#include <cmath>
#include <algorithm>

enum class OptionType { Call, Put };

inline double payoff_european(OptionType type, double ST, double K) {
    if (type == OptionType::Call) return std::max(ST - K, 0.0);
    return std::max(K - ST, 0.0);
}

// Exact GBM step under risk-neutral drift r.
inline double gbm_step_exact(double S, double r, double sigma, double dt, double Z) {
    const double drift = (r - 0.5 * sigma * sigma) * dt;
    const double diff  = sigma * std::sqrt(dt) * Z;
    return S * std::exp(drift + diff);
}

struct MCResult {
    double price = 0.0;
    double std_err = 0.0;      // standard error of discounted payoff mean
    double ci_lo = 0.0;       // 95% approx
    double ci_hi = 0.0;
};

inline MCResult price_european_mc(
    OptionType type,
    double S0, double K, double r, double sigma, double T,
    int steps, int paths,
    bool antithetic,
    struct RNG& rng
) 

{
    const double dt = T / std::max(steps, 1);
    const double disc = std::exp(-r * T);

    // We’ll compute mean & variance of discounted payoffs via Welford
    double mean = 0.0;
    double m2 = 0.0;
    int n = 0;

    auto push_sample = [&](double x) {
        ++n;
        double delta = x - mean;
        mean += delta / n;
        double delta2 = x - mean;
        m2 += delta * delta2;
    };

    const int base_paths = antithetic ? (paths / 2) : paths;

    for (int i = 0; i < base_paths; ++i) {
        // normal draw(s)
        // For antithetic, use Z and -Z in each step.
        double S_a = S0;
        double S_b = S0;

        for (int k = 0; k < steps; ++k) {
            const double Z = rng.Z();
            S_a = gbm_step_exact(S_a, r, sigma, dt, Z);
            if (antithetic) S_b = gbm_step_exact(S_b, r, sigma, dt, -Z);
        }

        const double pa = disc * payoff_european(type, S_a, K);
        push_sample(pa);

        if (antithetic && (n < paths)) {
            const double pb = disc * payoff_european(type, S_b, K);
            push_sample(pb);
        }
    }

    MCResult res;
    res.price = mean;

    const double var = (n > 1) ? (m2 / (n - 1)) : 0.0;
    res.std_err = std::sqrt(var / std::max(n, 1));
    const double z = 1.96; // 95% normal approx
    res.ci_lo = res.price - z * res.std_err;
    res.ci_hi = res.price + z * res.std_err;
    return res;
}
