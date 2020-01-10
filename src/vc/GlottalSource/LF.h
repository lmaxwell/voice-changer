#ifndef VC_VC_GLOTTALSOURCE_LF_H
#define VC_VC_GLOTTALSOURCE_LF_H

#include <Eigen/Dense>
#include "../Sinusoidal/Sin.h"

struct LF_Frame {
    double f0;
    double Rd;
};

void Rd_msp(const std::vector<SinFrame> & frames, double fs);

#endif // VC_VC_GLOTTALSOURCE_LF_H

