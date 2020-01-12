#include "../Signal/Resample.h"
#include "../Signal/Window.h"
#include "../FFT/FFT.h"
#include "Pitch.h"

#include <iostream>

using namespace Eigen;

void Pitch::estimate(const ArrayXd & x, int fs, int f0min, int f0max, int hopsize, ArrayXd & pitch, ArrayXi & voicing, ArrayXi & time)
{
    // Low pass filter.
    /*constexpr double fc = 100.0;
    const double alpha = (2.0 * M_PI * fc / fs) / (2.0 * M_PI * fc / fs + 1);

    ArrayXd y(x.size());
    y(0) = x(0);
    for (int i = 1; i < x.size(); ++i) {
        y(i) = y(i - 1) + alpha * (x(i) - y(i - 1));
    }*/
    ArrayXd y = x;

    // Create frame matrix.
    const int len = y.size();
    const int frameDuration = std::round((std::round(100.0 / 1000.0 * fs) - 2) / 2) * 2;
    const int shift = std::round(hopsize / 1000.0 * fs);
    const int halfDur = frameDuration / 2;

    const ArrayXd win = Window::createBlackmanHarris(frameDuration);

    std::vector<int> timeVec;
    std::vector<ArrayXd> frames;
    int t = 0;
    while (t < len - frameDuration) {
        timeVec.push_back(t + halfDur);

        ArrayXd frame = y.segment(t, frameDuration);
        frame *= win;
        frame -= frame.mean();

        frames.push_back(std::move(frame));

        t += shift;
    }

    const int N = timeVec.size();

    time = Map<ArrayXi>(timeVec.data(), N);

    Pitch::estimate_SWIPE(frames, fs, pitch);

    voicing.resize(N);
    for (int i = 0; i < N; ++i) {
        voicing(i) = (pitch(i) > 0.0) ? 1 : 0;
    }
}

