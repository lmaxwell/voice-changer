#include <iostream>
#include "../FFT/FFT.h"
#include "LF.h"

using namespace Eigen;

static double optimfnRd(double Rd, double fs, double f0, const ArrayXcd & M);

void Rd_msp(const std::vector<SinFrame> & frames, double fs, std::vector<LF_Frame> & lff)
{
    constexpr double Rdmin = 0.3;
    constexpr double Rdmax = 2.5;
    constexpr double Rdstep = 0.1;

    const int nFrames = frames.size();

    if (lff.size() != nFrames) {
        lff.resize(nFrames);
    }

    for (int iframe = 0; iframe < nFrames; ++iframe) {
        
        const auto & sf = frames.at(iframe);
        auto & lf = lff.at(iframe);

        lf.f0 = sf.f0;
        lf.Rd = NAN;

        const double f0 = sf.f0;

        ArrayXcd M;
        sin2shm(sf.sins, M);

        if (M.size() < 9)  {
            std::cerr << "The order of the harmonic model is too low for a reliable Rd estimate." << std::endl;
        }

        if (f0 == 0.0) {
            std::cout << "#" << (iframe + 1) << ": o" << std::endl;
            continue;
        }

        // Grid search.
        
        std::vector<std::pair<double, double>> E;

        for (double Rd = Rdmin; Rd <= Rdmax; Rd += Rdstep) {
            E.emplace_back(Rd, optimfnRd(Rd, fs, f0, M));
        }

        auto it = std::min_element(E.cbegin(), E.cend(),
                [](const auto & a, const auto & b) -> bool { return a.second < b.second; });
        
        lf.Rd = it->first;
       
        std::cout << "#" << (iframe + 1) << ": (f0, Rd) = (" << lf.f0 << ", " << lf.Rd << ")" << std::endl;
    }
}

static double optimfnRd(double Rd, double fs, double f0, const ArrayXcd & M)
{
    double te, tp, ta;
    Rd2tetpta(Rd, &te, &tp, &ta);

    // Compute the glottal model frequency response.
    ArrayXd f;
    f.setLinSpaced(M.size() - 1, 1, M.size() - 1);

    ArrayXcd G(f.size() + 1);
    G(0) = std::numeric_limits<double>::epsilon();
    lfGfmSpec(f, fs, 1.0 / f0, 1.0, te, tp, ta, G(seq(1, last)));

    // Compute the residual.
    ArrayXcd Nh = M / G;
    Nh(Nh.size() - 1) = abs(Nh(Nh.size() - 2));
    Nh(0) = abs(Nh(1));
    
    ArrayXcd N((Nh.size() - 1) * 2);
    N << Nh, conj(Nh(seq(1, last - 1)).reverse());

    // Calculate minimum phase spectrum from the cepstrum.
    const int ln = (Nh.size() - 1) * 2;
    irfft_plan(ln);
    Map<ArrayXd>(irfft_in(ln), ln) = log(abs(N));
    irfft(ln);
    Map<ArrayXd> irout(irfft_out(ln), ln);

    rfft_plan(ln);
    Map<ArrayXd> rin(rfft_in(ln), ln);
    rin.setZero();
    rin(0) = irout(0);
    rin(seq(1, ln / 2)) = irout(seq(1, ln / 2));
    rin(ln / 2 + 1) = irout(ln / 2 + 1);

    rfft(ln);

    ArrayXcd R = N / exp(Map<ArrayXd>(rfft_out(ln), ln)).cast<dcomplex>();
    const int nbh = std::min<int>(8, R.size());

    // Extract the phase.
    ArrayXd P = arg(R(seq(1, nbh -1)));
    
    // compute the MSPD2 error.
    double err = 0.0;
    
    for (int i = 0; i < P.size() - 1; ++i) {
        double angle = P(i + 1) - P(i) - P(0);
        // Wrap the angle around [-pi,pi]
        angle = fmod(angle + M_PI, 2 * M_PI) - M_PI;
        // Calculate the mean square phase difference.
        err += angle * angle;
    }

    err /= (double) (P.size() - 1);

    return err;

}
