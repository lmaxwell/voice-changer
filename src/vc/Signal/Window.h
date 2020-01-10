#ifndef VC_VC_SIGNAL_WINDOW_H
#define VC_VC_SIGNAL_WINDOW_H

#include <Eigen/Dense>

namespace Window {

    Eigen::ArrayXd createHamming(int size);
    Eigen::ArrayXd createHanning(int size);
    Eigen::ArrayXd createBlackmanHarris(int size);
    Eigen::ArrayXd createGaussian(int size);
    Eigen::ArrayXd createKaiser(int size);

}

#endif // VC_VC_SIGNAL_WINDOW_H
