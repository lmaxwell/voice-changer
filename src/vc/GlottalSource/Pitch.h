#ifndef VC_VC_GLOTTALSOURCE_PITCH_H
#define VC_VC_GLOTTALSOURCE_PITCH_H

#include <Eigen/Dense>

namespace Pitch {

    void estimate(const Eigen::ArrayXd & x, int fs, int f0min, int f0max, int hopsize, Eigen::ArrayXd & pitch, Eigen::ArrayXi & voicing, Eigen::ArrayXi & time);

    void estimate_MPM(const std::vector<Eigen::ArrayXd> & frames, double fs, Eigen::ArrayXd & pitch);

    void estimate_SWIPE(const std::vector<Eigen::ArrayXd> & frames, double fs, Eigen::ArrayXd & pitch);

}

#endif // VC_VC_GLOTTALSOURCE_PITCH_H
