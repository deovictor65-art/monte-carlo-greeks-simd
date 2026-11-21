#pragma once

#include <cmath>
#include <numbers>

namespace derivatives {

inline double standard_normal_cdf(double x) noexcept {
    return 0.5 * std::erfc(-x * std::numbers::inv_sqrt2);
}

inline double standard_normal_pdf(double x) noexcept {
    return (1.0 / std::sqrt(2.0 * std::numbers::pi)) * std::exp(-0.5 * x * x);
}

struct Greeks {
    double price{0.0};
    double delta{0.0};
    double gamma{0.0};
    double vega{0.0};
    double theta{0.0};
    double rho{0.0};
};

class BlackScholes {
public:
    static Greeks call_greeks(double S, double K, double r, double sigma, double T) noexcept {
        if (T <= 0.0 || sigma <= 0.0) {
            double intrinsic = std::max(0.0, S - K);
            return Greeks{intrinsic, S > K ? 1.0 : 0.0, 0.0, 0.0, 0.0, 0.0};
        }

        double sqrt_T = std::sqrt(T);
        double d1 = (std::log(S / K) + (r + 0.5 * sigma * sigma) * T) / (sigma * sqrt_T);
        double d2 = d1 - sigma * sqrt_T;

        double N_d1 = standard_normal_cdf(d1);
        double N_d2 = standard_normal_cdf(d2);
        double phi_d1 = standard_normal_pdf(d1);
        double discount = std::exp(-r * T);

        double price = S * N_d1 - K * discount * N_d2;
        double delta = N_d1;
        double gamma = phi_d1 / (S * sigma * sqrt_T);
        double vega = S * phi_d1 * sqrt_T;
        double theta = -(S * phi_d1 * sigma) / (2.0 * sqrt_T) - r * K * discount * N_d2;
        double rho = K * T * discount * N_d2;

        return Greeks{price, delta, gamma, vega, theta, rho};
    }

    static Greeks put_greeks(double S, double K, double r, double sigma, double T) noexcept {
        if (T <= 0.0 || sigma <= 0.0) {
            double intrinsic = std::max(0.0, K - S);
            return Greeks{intrinsic, S < K ? -1.0 : 0.0, 0.0, 0.0, 0.0, 0.0};
        }

        double sqrt_T = std::sqrt(T);
        double d1 = (std::log(S / K) + (r + 0.5 * sigma * sigma) * T) / (sigma * sqrt_T);
        double d2 = d1 - sigma * sqrt_T;

        double N_minus_d1 = standard_normal_cdf(-d1);
        double N_minus_d2 = standard_normal_cdf(-d2);
        double phi_d1 = standard_normal_pdf(d1);
        double discount = std::exp(-r * T);

        double price = K * discount * N_minus_d2 - S * N_minus_d1;
        double delta = N_minus_d1 - 1.0;
        double gamma = phi_d1 / (S * sigma * sqrt_T);
        double vega = S * phi_d1 * sqrt_T;
        double theta = -(S * phi_d1 * sigma) / (2.0 * sqrt_T) + r * K * discount * N_minus_d2;
        double rho = -K * T * discount * N_minus_d2;

        return Greeks{price, delta, gamma, vega, theta, rho};
    }
};

} // namespace derivatives
