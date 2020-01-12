#include "LF.h"

template<typename Func, typename Func2>
static double fzero(Func f, Func2 df, double x0);

void lfEpsAlpha(double T0, double te, double tp, double ta, double * outa, double * oute)
{
    const double Te = te * T0;
    const double Tp = tp * T0;
    const double Ta = ta * T0;
    const double wg = M_PI / Tp;

    // e is expressed by an implicit equation
    const auto fb = [&](double e) {
        return 1.0 - exp(-e * (T0 - Te)) - e * Ta;
    };
    const auto dfb = [&](double e) {
        return (T0 - Te) * exp(-e * (T0 - Te)) - Ta;
    };
    const double e = fzero(fb, dfb, 1 / (Ta + 1e-13));

    // a is expressed by another implicit equation
    // integral{0, T0} ULF(t) dt, where ULF(t) is the LF model equation
    const double A = (1.0 - exp(-e * (T0 - Te))) / (e * e * Ta) - (T0 - Te) * exp(-e * (T0 - Te)) / (e * Ta);
    const auto fa = [&](double a) {
        return (a * a + wg * wg) * sin(wg * Te) * A + wg * exp(-a * Te) + a * sin(wg * Te) - wg * cos(wg * Te);
    };
    const auto dfa = [&](double a) {
        return (2 * A * a + 1) * sin(wg * Te) - wg * Te * exp(-a * Te);
    };
    const double a = fzero(fa, dfa, 4.42);

    *oute = e;
    *outa = a;
}

template<typename Func, typename Func2>
static double fzero(Func f, Func2 df, double x0)
{
    constexpr double tol = 1e-7;
    constexpr double eps = 1e-13;
    constexpr int maxIter = 30;

    for (int iter = 0; iter < maxIter; ++iter) 
    {
        double y = f(x0);
        double dy = df(x0);

        if (abs(dy) < eps) {
            return NAN;
        }

        double x1 = x0 - y / dy;

        if (abs(x1 - x0) <= tol) {
            return x1;
        }

        x0 = x1;
    }

    return x0;
}

