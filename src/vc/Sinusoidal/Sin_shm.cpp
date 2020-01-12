#include "Sin.h"

using namespace Eigen;

void sin2shm(const ArrayX5d & sins, ArrayXcd & M)
{
    constexpr double maxFreq = 8000; 

    // Filter out harmonics that are higher than maxFreq.
    int nbh = 0;
    while (nbh < sins.rows() && sins(nbh, 0) <= maxFreq) {
        nbh++;
    }

    // Create spectrum.
    ArrayXcd Mh = sins(seq(0, nbh - 1), 1) * exp(dcomplex(0.0, 1.0) * sins(seq(0, nbh - 1), 2));
    Mh(0) = dcomplex(sign(real(Mh(0))) * abs(Mh(0)));
    Mh(nbh - 1) = dcomplex(sign(real(Mh(last))) * abs(Mh(nbh - 1)));

    M.resize((Mh.size() - 1) * 2);
    M << Mh, conj(Mh(seq(1, last - 1)).reverse());

    M(0) = abs(M(1));
}
