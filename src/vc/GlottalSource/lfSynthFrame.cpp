#include "LF.h"

using namespace Eigen;

int lfSynthFrame(const LF_Frame & fr, double fs, ArrayXd & x, int start)
{
    const double T0 = 1.0 / fr.f0;
    const double Ee = 1;
     
    const int period = fs / fr.f0;

    double te, tp, ta;
    Rd2tetpta(fr.Rd, &te, &tp, &ta);

    double a, e;
    lfEpsAlpha(T0, te, tp, ta, &a, &e);

    const double Te = te * T0;
    const double Tp = tp * T0;
    const double Ta = ta * T0;

    for (int kt = 0; kt < period; ++kt) {
        if (start + kt < x.size()) {
            double t = (kt * T0) / (double) (period - 1);
        
            if (t <= Te) { 
                x(start + kt) += (-Ee * exp(a * (t - Te)) * sin(M_PI * t / Tp)) / sin(M_PI * Te / Tp);
            }
            else {
                x(start + kt) += -Ee / (e * Ta) * (exp(-e * (t - Te)) - exp(-e * (T0 - Te)));
            }
        }
    }

    return start + period;
}
