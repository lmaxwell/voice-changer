#include "EKF.h"

using namespace Eigen;

static double ekf_alpha_one(Ref<const ArrayXd> gfd, int period, double Ee, double Tp, double Te, double a0)
{
    constexpr double Ro = 0.01;
    constexpr double Po = 1;

    const int No = (int) (period * Te);

    double ak = a0;
    double Pk = Po;

    for (int k = 0; k < No; ++k) {
        double akm = ak;
        double Pkm = Pk;

        double ho = lf_ho(akm, k, period, Ee, Tp, Te);
        double Ho = da_lf_ho(akm, k, period, Ee, Tp, Te);

        double Kk = Pkm * Ho / (Ho * Pkm * Ho + Ro);
        
        ak = akm + Kk * (gfd(k) - ho);
        Pk = (1 - Kk * Ho) * Pkm;
    }

    return ak;
}

double optlf_alpha(Ref<const ArrayXd> gfd, int period, double Ee, double Tp, double Te, double a0min, double a0max, double a0step)
{
    double bestAlpha;
    double bestMMSE = HUGE_VAL;

    const int No = (int) (Te * period);

    for (double a0 = a0min; a0 <= a0max; a0 += a0step) {

        // Estimate alpha from the selected alpha_0.
        double a = ekf_alpha_one(gfd, period, Ee, Tp, Te, a0);

        // Reconstruct the open phase with this value of alpha.
        double mmse = 0.0;
        for (int k = 0; k < No; ++k) {
            double val = gfd(k) - lf_ho(a, k, period, Ee, Tp, Te);
            mmse += val * val;
        }
        // Since we're looking for the min, no need to make it a mean.
        //mmse /= (double) No;

        // Update the best candidate if needed.
        if (mmse < bestMMSE) {
            bestAlpha = a;
            bestMMSE = mmse;
        }
    }

    return bestAlpha;
}
