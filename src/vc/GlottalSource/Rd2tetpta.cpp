#include "LF.h"

void Rd2tetpta(double Rd, double * te, double * tp, double * ta)
{
    const double Rap = (-1.0 + 4.8 * Rd) / 100.0;
    const double Rkp = (22.4 + 11.8 * Rd) / 100.0;
    const double Rgp = 1.0 / (4.0 * ((0.11 * Rd / (0.5 + 1.2 * Rkp)) - Rap) / Rkp);

    *tp = 1.0 / (2.0 * Rgp);
    *te = *tp * (Rkp + 1.0);
    *ta = Rap;
}
