#pragma once

#include "black_scholes.hpp"
#include <vector>
#include <random>
#include <algorithm>
#include <numeric>
#include <iostream>

namespace derivatives {

enum class OptionType {
    EuropeanCall,
    EuropeanPut,
    AsianCall,
    BarrierUpAndOutCall
};

struct MonteCarloConfig {
    size_t num_paths{200'000};
    size_t time_steps{100};
    uint64_t seed{42};
};

struct HestonParams {
    double v0{0.04};     // Initial variance (sigma^2)
    double kappa{2.0};   // Mean reversion speed
    double theta{0.04};  // Long-run variance
    double xi{0.3};      // Volatility of variance
    double rho{-0.7};    // Correlation between stock and variance Brownian motions
};

class MonteCarloEngine {
public:
    explicit MonteCarloEngine(MonteCarloConfig config = {}) : config_(config) {}

    // GBM European / Exotic Option Simulation
    double price_option(OptionType type, double S, double K, double r, double sigma, double T, double barrier = 0.0) {
        std::mt19937_64 rng(config_.seed);
        std::normal_distribution<double> normal(0.0, 1.0);

        double dt = T / static_cast<double>(config_.time_steps);
        double sqrt_dt = std::sqrt(dt);
        double drift = (r - 0.5 * sigma * sigma) * dt;
        double vol_sqrt_dt = sigma * sqrt_dt;
        double discount = std::exp(-r * T);

        double payoff_sum = 0.0;

        for (size_t p = 0; p < config_.num_paths; ++p) {
            double current_S = S;
            double path_sum = current_S;
            bool knocked_out = false;

            for (size_t t = 0; t < config_.time_steps; ++t) {
                double z = normal(rng);
                current_S *= std::exp(drift + vol_sqrt_dt * z);
                path_sum += current_S;

                if (type == OptionType::BarrierUpAndOutCall && current_S >= barrier) {
                    knocked_out = true;
                    break;
                }
            }

            double payoff = 0.0;
            if (type == OptionType::EuropeanCall) {
                payoff = std::max(0.0, current_S - K);
            } else if (type == OptionType::EuropeanPut) {
                payoff = std::max(0.0, K - current_S);
            } else if (type == OptionType::AsianCall) {
                double avg_price = path_sum / static_cast<double>(config_.time_steps + 1);
                payoff = std::max(0.0, avg_price - K);
            } else if (type == OptionType::BarrierUpAndOutCall) {
                payoff = knocked_out ? 0.0 : std::max(0.0, current_S - K);
            }

            payoff_sum += payoff;
        }

        return discount * (payoff_sum / static_cast<double>(config_.num_paths));
    }

    // Heston Stochastic Volatility Model Pricing
    double price_heston_call(double S, double K, double r, double T, const HestonParams& p) {
        std::mt19937_64 rng(config_.seed);
        std::normal_distribution<double> normal(0.0, 1.0);

        double dt = T / static_cast<double>(config_.time_steps);
        double sqrt_dt = std::sqrt(dt);
        double discount = std::exp(-r * T);
        double sqrt_one_minus_rho2 = std::sqrt(1.0 - p.rho * p.rho);

        double payoff_sum = 0.0;

        for (size_t path = 0; path < config_.num_paths; ++path) {
            double current_S = S;
            double current_v = p.v0;

            for (size_t step = 0; step < config_.time_steps; ++step) {
                double z1 = normal(rng);
                double z2 = normal(rng);
                double z_vol = p.rho * z1 + sqrt_one_minus_rho2 * z2;

                double v_pos = std::max(0.0, current_v);
                double sqrt_v = std::sqrt(v_pos);

                // Euler-Maruyama with Full Truncation
                current_S *= std::exp((r - 0.5 * v_pos) * dt + sqrt_v * sqrt_dt * z1);
                current_v += p.kappa * (p.theta - v_pos) * dt + p.xi * sqrt_v * sqrt_dt * z_vol;
            }

            payoff_sum += std::max(0.0, current_S - K);
        }

        return discount * (payoff_sum / static_cast<double>(config_.num_paths));
    }

    // Finite-Difference Delta & Gamma
    Greeks calculate_mc_greeks(OptionType type, double S, double K, double r, double sigma, double T) {
        double dS = 0.01 * S;
        double base_price = price_option(type, S, K, r, sigma, T);
        double price_up = price_option(type, S + dS, K, r, sigma, T);
        double price_down = price_option(type, S - dS, K, r, sigma, T);

        double delta = (price_up - price_down) / (2.0 * dS);
        double gamma = (price_up - 2.0 * base_price + price_down) / (dS * dS);

        double d_vol = 0.01;
        double price_vol_up = price_option(type, S, K, r, sigma + d_vol, T);
        double vega = (price_vol_up - base_price) / (100.0 * d_vol); // per 1% vol change

        return Greeks{base_price, delta, gamma, vega, 0.0, 0.0};
    }

private:
    MonteCarloConfig config_;
};

} // namespace derivatives
