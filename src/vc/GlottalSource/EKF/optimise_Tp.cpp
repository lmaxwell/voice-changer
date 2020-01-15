#include "EKF.h"

using namespace Eigen;

std::pair<double, double> optlf_Tp_and_alpha(Ref<const ArrayXd> gfd, int period, double Ee, double Tp0, double Te, double a0min, double a0max, double a0step)
{
    const double Tpmin = Tp0 - 0.05;
    const double Tpmax = Tp0 + 0.05;
    const double Tpstep = 0.01;

    double bestTp;
    double bestAlpha;
    double bestMMSE = HUGE_VAL;

    const int No = (int) (Te * period);

    for (double Tp = Tpmin; Tp <= Tpmax; Tp += Tpstep) {
        
        // Estimate alpha from the selected Tp.
        double a = optlf_alpha(gfd, period, Ee, Tp, Te, a0min, a0max, a0step);
        
        // Reconstruct the open phase with these values of Tp and alpha.
        double mmse = 0.0;
        for (int k = 0; k < No; ++k) {
            double val = gfd(k) - lf_ho(a, k, period, Ee, Tp, Te);
            mmse += val * val;
        }
        // Since we're looking for the min, no need to make it a mean.
        //mmse /= (double) No;

        // Update the best candidate if needed.
        if (mmse < bestMMSE) {
            bestTp = Tp;
            bestAlpha = a;
            bestMMSE = mmse;
        }
    }

    return std::make_pair(bestTp, bestAlpha);

}
