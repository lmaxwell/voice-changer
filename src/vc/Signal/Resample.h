#ifndef VC_VC_SIGNAL_RESAMPLE_H
#define VC_VC_SIGNAL_RESAMPLE_H

#include <Eigen/Dense>

namespace Resample {

    enum {
        Nearest = 0,
        Linear = 1,
        Cubic = 2
    };

    Eigen::ArrayXd resample(const Eigen::ArrayXd & x, double sourceFs, double targetFs, int precision);

    Eigen::ArrayXd upsample(const Eigen::ArrayXd & x);

    double interpolate_sinc(const Eigen::ArrayXd & y, double x, int maxDepth);

}

#endif // VC_VC_SIGNAL_RESAMPLE_H
