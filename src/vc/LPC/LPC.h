#ifndef VC_VC_LPC_LPC_H
#define VC_VC_LPC_LPC_H

#include <Eigen/Dense>

void lpcFrame(const Eigen::ArrayXd & x, int order, Eigen::ArrayXd & a);

void lpcResidual(const Eigen::ArrayXd & x, int L, int shift, int order, Eigen::ArrayXd & res);

#endif // VC_VC_LPC_LPC_H
