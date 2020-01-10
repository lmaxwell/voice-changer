#ifndef VC_VC_GLOTTALSOURCE_PITCH_H
#define VC_VC_GLOTTALSOURCE_PITCH_H

#include <Eigen/Dense>

namespace Pitch {

    void estimate_SRH(const Eigen::ArrayXd & x, int fs, int f0min, int f0max, int hopsize, Eigen::ArrayXd & pitch, Eigen::ArrayXi & voicing, Eigen::ArrayXi & time);

}

#endif // VC_VC_GLOTTALSOURCE_PITCH_H
