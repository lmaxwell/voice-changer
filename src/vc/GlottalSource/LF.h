#ifndef VC_VC_GLOTTALSOURCE_LF_H
#define VC_VC_GLOTTALSOURCE_LF_H

#include <Eigen/Dense>
#include "../Sinusoidal/Sin.h"

struct LF_Frame {
    double f0;
    double Rd;
};

void lfEpsAlpha(double T0, double te, double tp, double ta, double * a, double * e);

void lfGfmSpec(const Eigen::ArrayXd & f, double fs, double T0, double Ee, double te, double tp, double ta, Eigen::Ref<Eigen::ArrayXcd> G);

void lfSynth(const std::vector<LF_Frame> & lff, const Eigen::ArrayXi & voicing, const Eigen::ArrayXi & time, double fs, Eigen::ArrayXd & x);
int lfSynthFrame(const LF_Frame & fr, double fs, Eigen::ArrayXd & x, int start);

void Rd2tetpta(double Rd, double * te, double * tp, double * ta);

void Rd_msp(const std::vector<SinFrame> & frames, double fs, std::vector<LF_Frame> & lff);

#endif // VC_VC_GLOTTALSOURCE_LF_H

