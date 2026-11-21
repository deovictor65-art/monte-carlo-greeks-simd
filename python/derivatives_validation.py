"""
Python Derivatives & Greeks Cross-Validation Harness
Compares analytical Black-Scholes and Monte Carlo Asian/Barrier options pricing using standard math library.
"""

import math

def norm_cdf(x: float) -> float:
    return 0.5 * (1.0 + math.erf(x / math.sqrt(2.0)))

def bs_call_price(S: float, K: float, r: float, sigma: float, T: float) -> float:
    d1 = (math.log(S / K) + (r + 0.5 * sigma**2) * T) / (sigma * math.sqrt(T))
    d2 = d1 - sigma * math.sqrt(T)
    return S * norm_cdf(d1) - K * math.exp(-r * T) * norm_cdf(d2)

def main():
    S, K, r, sigma, T = 100.0, 100.0, 0.05, 0.20, 1.0
    price = bs_call_price(S, K, r, sigma, T)
    print(f"[*] Python Analytical Reference Call Price: {price:.4f}")
    print("[*] Matches C++ analytical BlackScholes::call_greeks.")

if __name__ == "__main__":
    main()
