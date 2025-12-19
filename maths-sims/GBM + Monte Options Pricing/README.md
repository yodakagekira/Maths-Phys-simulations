# Monte Carlo Option Pricing (GBM)

An interactive **Monte Carlo option pricing simulator** written in **C++ (C++20)** using **raylib** for real-time visualisation.

This project demonstrates applied probability, stochastic modelling, and numerical methods used in quantitative finance, with a focus on **Geometric Brownian Motion (GBM)** and **risk-neutral valuation**.

---

## Features

- **Geometric Brownian Motion (GBM)**
  - Exact lognormal discretisation
  - Risk-neutral drift
  - Multiple simulated price paths (fan chart)

- **Monte Carlo Option Pricing**
  - European call and put options
  - Discounted payoff estimation
  - 95% confidence intervals

- **Variance Reduction**
  - Antithetic variates (toggleable)

- **Real-time Visualisation**
  - GBM path fan chart
  - Monte Carlo convergence plot (price vs number of paths)
  - Interactive parameter sliders

---

## Mathematical Model

### Geometric Brownian Motion
Under the risk-neutral measure, the asset price follows:

dS = r S dt + σ S dW

with exact discretisation:

S(t + Δt) = S(t) · exp[(r − ½σ²)Δt + σ√Δt Z],  Z ~ N(0,1)

### Monte Carlo Estimator

For a European option with payoff Π(S_T):

V̂ = e^(−rT) · (1/N) Σ Π(S_Tᵢ)

A normal approximation is used to compute 95% confidence intervals.

---

## Controls

Adjust parameters in real time:

- Initial price S₀
- Strike price K
- Risk-free rate r
- Volatility σ
- Maturity T
- Time steps per path
- Number of Monte Carlo paths
- Option type (Call / Put)
- Antithetic variates toggle

---

The implementation is intentionally lightweight and avoids external plotting libraries in favour of direct rendering via raylib.

## Possible Extensions

- Control variates using Black–Scholes closed-form solution
- Greeks via finite differences
- Parallel Monte Carlo (OpenMP)
- CSV export for offline analysis
- Comparison with binomial tree pricing


## Author

Ahmad Abdullah  
BSc Mathematics & Computer Science

