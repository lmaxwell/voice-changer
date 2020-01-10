#ifndef VC_VC_SINUSOIDAL_SIN_H
#define VC_VC_SINUSOIDAL_SIN_H

#include <Eigen/Dense>

using ArrayX5d = Eigen::Array<double, Eigen::Dynamic, 5>;

struct SinFrame {
    double f0;
    ArrayX5d sins;
};

void sinAnalysis(const Eigen::ArrayXd & x, double fs, const Eigen::ArrayXd & f0s, const Eigen::ArrayXi & times, std::vector<SinFrame> & frames);

#endif // VC_VC_SINUSOIDAL_SIN_H
