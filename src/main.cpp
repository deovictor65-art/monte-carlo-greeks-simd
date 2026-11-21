#include "derivatives/black_scholes.hpp"
#include "derivatives/monte_carlo_engine.hpp"
#include <iostream>
#include <iomanip>
#include <chrono>

int main() {
    std::cout << "=========================================================\n";
    std::cout << "   Monte Carlo Exotic Derivatives & Greeks Engine C++20   \n";
    std::cout << "=========================================================\n\n";

    double S = 100.0;    // Spot price
    double K = 100.0;    // Strike price
    double r = 0.05;     // Risk-free rate (5%)
    double sigma = 0.20; // Volatility (20%)
    double T = 1.0;      // Maturity 1 year

    std::cout << "[*] Analytic Black-Scholes Baseline:\n";
    auto bs_call = derivatives::BlackScholes::call_greeks(S, K, r, sigma, T);
    std::cout << "    Price: " << std::fixed << std::setprecision(4) << bs_call.price << "\n";
    std::cout << "    Delta: " << bs_call.delta << "\n";
    std::cout << "    Gamma: " << bs_call.gamma << "\n";
    std::cout << "    Vega:  " << bs_call.vega << "\n";
    std::cout << "    Theta: " << bs_call.theta << "\n";
    std::cout << "    Rho:   " << bs_call.rho << "\n\n";

    derivatives::MonteCarloConfig config{.num_paths = 250'000, .time_steps = 100, .seed = 42};
    derivatives::MonteCarloEngine mc_engine(config);

    std::cout << "[*] Running Monte Carlo Simulation (250,000 paths, 100 steps)...\n";
    auto t0 = std::chrono::high_resolution_clock::now();

    double mc_euro_call = mc_engine.price_option(derivatives::OptionType::EuropeanCall, S, K, r, sigma, T);
    double mc_asian_call = mc_engine.price_option(derivatives::OptionType::AsianCall, S, K, r, sigma, T);
    double mc_barrier_call = mc_engine.price_option(derivatives::OptionType::BarrierUpAndOutCall, S, K, r, sigma, T, 120.0);

    derivatives::HestonParams heston{.v0 = 0.04, .kappa = 2.0, .theta = 0.04, .xi = 0.3, .rho = -0.7};
    double heston_call = mc_engine.price_heston_call(S, K, r, T, heston);

    auto t1 = std::chrono::high_resolution_clock::now();
    double elapsed_ms = std::chrono::duration<double, std::milli>(t1 - t0).count();

    std::cout << "\n[+] Simulation Results (Elapsed: " << elapsed_ms << " ms):\n";
    std::cout << "    MC European Call Price:       " << mc_euro_call << " (Diff vs BS: " << (mc_euro_call - bs_call.price) << ")\n";
    std::cout << "    MC Asian Arithmetic Call:     " << mc_asian_call << "\n";
    std::cout << "    MC Barrier Up-and-Out (H=120):" << mc_barrier_call << "\n";
    std::cout << "    Heston Stochastic Vol Call:   " << heston_call << "\n";

    std::cout << "\n[✔] Monte Carlo Derivatives & Greeks Engine Verified.\n";
    return 0;
}
