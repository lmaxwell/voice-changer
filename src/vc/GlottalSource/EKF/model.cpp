#include "EKF.h"

double lf_ho(double a, int k, int period, double Ee, double Tp, double Te)
{
    const int N = period - 1;
    const double t = (double) k / (double) N;
    return (-Ee * exp(a * (t - Te)) * sin(M_PI * t / Tp)) / sin(M_PI * Te / Tp);
}

double da_lf_ho(double a, int k, int period, double Ee, double Tp, double Te)
{
    const int N = period - 1;
    const double t = (double) k / (double) N;
    return (-Ee * (t - Te) * exp(a * (t - Te)) * sin(M_PI * t / Tp)) / sin(M_PI * Te / Tp);
}


double lf_hr(double e, int k, int period, double Ee, double Ta, double Te)
{
    const int N = period - 1;
    const double t = (double) k / (double) N;
    return -Ee / (e * Ta) * (exp(-e * (t - Te)) - exp(-e * (1 - Te)));
}

double de_lf_hr(double e, int k, int period, double Ee, double Ta, double Te)
{
    const int N = period - 1;
    const double t = (double) k / (double) N;

    const double u = -Ee / (e * Ta);
    const double du = -u / e;

    const double v1 = exp(-e * (t - Te));
    const double dv1 = -(t - Te) * v1;

    const double v2 = exp(-e * (1 - Te));
    const double dv2 = -(1 - Te) * v2;

    return u * (dv1 + dv2) + du * (v1 + v2);
}
