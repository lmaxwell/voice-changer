#include <cfloat>
#include "Pitch.h"
#include "McLeod/MPM.h"

using namespace Eigen;

constexpr double cutoff = 0.97;
constexpr double smallCutoff = 0.5;
constexpr double lowerPitchCutoff = 60.0;

void Pitch::estimate_MPM(const std::vector<ArrayXd> & frames, double fs, ArrayXd & pitch)
{
    pitch.setZero(frames.size());

    for (int i = 0; i < frames.size(); ++i) {
    
        const ArrayXd & x = frames.at(i);

        ArrayXd nsdf = MPM::autocorrelation(x);
        nsdf /= nsdf.abs().maxCoeff();

        std::vector<int> maxPositions = MPM::peakPicking(nsdf);
        std::vector<std::pair<double, double>> estimates;

        double highestAmplitude = -DBL_MAX;

        for (int i : maxPositions) {
            highestAmplitude = std::max(highestAmplitude, nsdf(i));
            if (nsdf(i) > smallCutoff) {
                auto est = MPM::parabolicInterpolation(nsdf, i);
                estimates.push_back(est);
                highestAmplitude = std::max(highestAmplitude, std::get<1>(est));
            }
        }

        if (estimates.empty()) {
            pitch(i) = 0;
            continue;
        }

        double actualCutoff = cutoff * highestAmplitude;
        double f0 = 0;

        for (auto it = estimates.crbegin(); it != estimates.crend(); ++it) {
            if (std::get<1>(*it) >= actualCutoff) {
                f0 = fs / std::get<0>(*it);

                if (f0 < lowerPitchCutoff)
                    continue;
                else
                    break;
            }
        }

        if (f0 >= lowerPitchCutoff) {
            pitch(i) = f0;
        }
        else {
            pitch(0) = 0;
        }

    }
}
