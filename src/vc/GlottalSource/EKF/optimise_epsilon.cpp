#include "EKF.h"

using namespace Eigen;

static double optlf_epsilon_one(Ref<const ArrayXd> gfd, int period, double Ee, double Ta, double Te, double e0)
{
    constexpr double Rr = 0.01;
    constexpr double P0 = 1;

    const int No = (int) (Te * period);
    
    double ek = e0;
    double Pk = P0;

    for (int k = No + 1; k < period; ++k) {
        double ekm = ek;
        double Pkm = Pk;

        double hr = lf_hr(ekm, k, period, Ee, Ta, Te);
        double Hr = de_lf_hr(ekm, k, period, Ee, Ta, Te);

        double Kk = Pkm * Hr / (Hr * Pkm * Hr + Rr);
        
        ek = ekm + Kk * (gfd(k) - hr);
        Pk = (1 - Kk * Hr) * Pkm;
    }

    return ek;
}

double optlf_epsilon(Ref<const ArrayXd> gfd, int period, double Ee, double Ta, double Te, double e0min, double e0max, double e0step)
{
    double bestEpsilon;
    double bestMMSE = HUGE_VAL;

    const int No = (int) (Te * period);

    for (double e0 = e0min; e0 <= e0max; e0 += e0step) {

        // Estimate epsilon from the selected epsilon_0.
        double e = optlf_epsilon_one(gfd, period, Ee, Ta, Te, e0);

        // Reconstruct the open phase with this value of alpha.
        double mmse = 0.0;
        for (int k = No + 1; k < period; ++k) {
            double val = gfd(k) - lf_hr(e, k, period, Ee, Ta, Te);
            mmse += val * val;
        }
        // Since we're looking for the min, no need to make it a mean.
        //mmse /= (double) (period - No);

        // Update the best candidate if needed.
        if (mmse < bestMMSE) {
            bestEpsilon = e;
            bestMMSE = mmse;
        }
    }

    return bestEpsilon;
}
