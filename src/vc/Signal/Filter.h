#ifndef VC_VC_SIGNAL_FILTER_H
#define VC_VC_SIGNAL_FILTER_H

#include <Eigen/Dense>

namespace Filter {

    void preEmphasis(Eigen::ArrayXd & x, double samplingFrequency, double preEmphasisFrequency);

    void apply(const Eigen::ArrayXd & b, const Eigen::ArrayXd & x, Eigen::ArrayXd & y);
    void apply(const Eigen::ArrayXd & b, const Eigen::ArrayXd & a, const Eigen::ArrayXd & x, Eigen::ArrayXd & y);

}

#endif // VC_VC_SIGNAL_FILTER_H
