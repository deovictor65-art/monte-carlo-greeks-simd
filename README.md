# Ultra-Fast Multi-Asset Exotic Derivatives & Greeks Engine (`monte-carlo-greeks-simd`)

A high-performance C++20 quantitative financial engineering library for exotic options pricing, stochastic volatility simulation, and high-precision Greeks computation ($\Delta, \Gamma, \mathcal{V}, \Theta, \rho$).

## Features

- **Analytic Black-Scholes Formulae**: Exact closed-form pricing and Greeks for European calls and puts with error-function standard normal distribution calculations.
- **Monte Carlo Engine**: Path-generation engine for European, Asian (arithmetic average), and Barrier (Up-and-Out) options.
- **Heston Stochastic Volatility**: Euler-Maruyama discretization with full truncation for mean-reverting variance and asset-variance correlation.
- **Finite-Difference Sensitivities**: Central finite-difference computation of Delta, Gamma, and Vega across arbitrary path-dependent payoffs.

## Quick Build & Run

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
ctest --test-dir build --output-on-failure
./build/derivatives_engine
```

## Python Validation

```bash
python python/derivatives_validation.py
```

## License

MIT License. Authored by deovictor65-art.
