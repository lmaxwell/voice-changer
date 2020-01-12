#include "LF.h"

using namespace Eigen;

void lfGfmSpec(const ArrayXd & f, double fs, double T0, double Ee, double te, double tp, double ta, Ref<ArrayXcd> G)
{
    const double Te = te * T0;
    const double Tp = tp * T0;
    const double Ta = ta * T0;
    const double wg = M_PI / Tp;

    double a, e;
    lfEpsAlpha(T0, te, tp, ta, &a, &e);

    const double E0 = -Ee / (exp(a * Te) * sin(wg * Te));
    
    ArrayXcd i2pif = dcomplex(0, 2 * M_PI) * f.cast<dcomplex>();

    ArrayXcd P1 = E0 * (1.0 / ((a - i2pif).square() + wg * wg));
    ArrayXcd P2 = (wg + exp((a - i2pif) * Te) * ((a - i2pif) * sin(wg * Te) - wg * cos(wg * Te)));
    ArrayXcd P3 = (Ee * (exp(-i2pif * Te) / ((e * Ta * i2pif) * (e + i2pif))));
    ArrayXcd P4 = (e * (1.0 - e * Ta) * (1.0 - exp(-i2pif * (T0 - Te))) - e * Ta * i2pif);

    G = fs * (P1 * P2 + P3 * P4);
}
