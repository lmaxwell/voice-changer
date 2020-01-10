#ifndef VC_VC_IAIF_IAIF_H
#define VC_VC_IAIF_IAIF_H

#include <Eigen/Dense>
#include <vector>

struct IAIF_Result {
    Eigen::ArrayXd g;
    int winLen, winShift;
    std::vector<Eigen::ArrayXd> ag;
    std::vector<Eigen::ArrayXd> av;
};

struct IAIF_Frame {
    Eigen::ArrayXd ag;
    Eigen::ArrayXd av;
    Eigen::ArrayXd g;
};

void iaif(IAIF_Result & result, Eigen::ArrayXd & x, double fs, double winLenMs, double winShiftMs, int p_vt, int p_gl, double d);
void iaifFrame(IAIF_Frame & frame, Eigen::ArrayXd & x, double fs, int p_vt, int p_gl, double d, const Eigen::ArrayXd & win);

#endif // VC_VC_IAIF_IAIF_H
