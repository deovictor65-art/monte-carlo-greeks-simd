#include "derivatives/black_scholes.hpp"
#include "derivatives/monte_carlo_engine.hpp"
#include <cassert>
#include <iostream>

void test_black_scholes_put_call_parity() {
    double S = 100.0, K = 100.0, r = 0.05, sigma = 0.2, T = 1.0;
    auto call = derivatives::BlackScholes::call_greeks(S, K, r, sigma, T);
    auto put = derivatives::BlackScholes::put_greeks(S, K, r, sigma, T);

    // Put-Call Parity: C - P = S - K * exp(-r * T)
    double discount_K = K * std::exp(-r * T);
    double lhs = call.price - put.price;
    double rhs = S - discount_K;

    assert(std::abs(lhs - rhs) < 1e-6);
    std::cout << "[PASS] Black-Scholes Put-Call Parity verified\n";
}

void test_mc_convergence_to_black_scholes() {
    double S = 100.0, K = 100.0, r = 0.05, sigma = 0.2, T = 1.0;
    auto bs_call = derivatives::BlackScholes::call_greeks(S, K, r, sigma, T);

    derivatives::MonteCarloConfig config{.num_paths = 100'000, .time_steps = 50, .seed = 123};
    derivatives::MonteCarloEngine mc(config);
    double mc_call = mc.price_option(derivatives::OptionType::EuropeanCall, S, K, r, sigma, T);

    // Monte Carlo error within 0.15 for 100k paths
    assert(std::abs(mc_call - bs_call.price) < 0.15);
    std::cout << "[PASS] Monte Carlo convergence to Black-Scholes analytical formula\n";
}

int main() {
    std::cout << "Running Derivatives & Greeks Unit Tests...\n";
    test_black_scholes_put_call_parity();
    test_mc_convergence_to_black_scholes();
    std::cout << "\nAll Unit Tests Passed Successfully!\n";
    return 0;
}
