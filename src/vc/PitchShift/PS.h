#ifndef VC_VC_PITCHSHIFT_PS_H
#define VC_VC_PITCHSHIFT_PS_H

#include <Eigen/Dense>
#include <vector>

Eigen::ArrayXd pitchShift(int lenx, int winLen, int winShift, std::vector<Eigen::ArrayXd> & Hgvec, double fs, int nfft, double factor);

#endif // VC_VC_PITCHSHIFT_PS_H
