#ifndef VC_VC_SINUSOIDAL_SIN_H
#define VC_VC_SINUSOIDAL_SIN_H

#include <Eigen/Dense>

using ArrayX5d = Eigen::Array<double, Eigen::Dynamic, 5>;

struct SinFrame {
    double f0;
    ArrayX5d sins;
};

void sinAnalysis(const Eigen::ArrayXd & x, double fs, const Eigen::ArrayXd & f0s, const Eigen::ArrayXi & times, std::vector<SinFrame> & frames);

void sin2shm(const ArrayX5d & sins, Eigen::ArrayXcd & M);

inline int nextpow2(int x) {
    if (x < 0)
        return 0;
    --x;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    return x + 1;
}

template<typename T>
inline int sign(T x) {
    return (x > T(0)) - (x < T(0));
}

#endif // VC_VC_SINUSOIDAL_SIN_H
