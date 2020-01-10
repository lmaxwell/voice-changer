#include "../Signal/Resample.h"
#include "../Signal/Window.h"
#include "../FFT/FFT.h"
#include "../LPC/LPC.h"
#include "Pitch.h"

using namespace Eigen;

static void SRH(const std::vector<ArrayXd> & specs, int nHarmonics, int f0min, int f0max, ArrayXd & f0, ArrayXd & srhVal);

void Pitch::estimate_SRH(const ArrayXd & x_, int fs, int f0min, int f0max, int hopsize, ArrayXd & pitch, ArrayXi & voicing, ArrayXi & time)
{
    ArrayXd x;

    // Resample to 16kHz if sample rate is higher than that.
    if (fs > 16000) {
        x = Resample::resample(x_, fs, 16000, 50);
        fs = 16000;
    }
    else {
        x = x_;
    }
    
    constexpr int nHarmonics = 5;
    constexpr double srhIterThresh = 0.1;
    constexpr double srhStdThresh = 0.05;
    constexpr double voicingThresh = 0.07;
    constexpr double voicingThresh2 = 0.085;

    const int lpcOrder = std::round(0.75 * fs / 1000.0);
    const int nIter = 2;

    // Compute LPC residual.
    ArrayXd res;
    lpcResidual(
        x,
        std::round(25.0 / 1000.0 * fs),
        std::round(5.0 / 1000.0 * fs),
        lpcOrder,
        res
    );

    // Create frame matrix.
    const int len = res.size();
    const int frameDuration = std::round((std::round(100.0 / 1000.0 * fs) - 2) / 2) * 2;
    const int shift = std::round(hopsize / 1000.0 * fs);
    const int halfDur = frameDuration / 2;

    const ArrayXd win = Window::createBlackmanHarris(frameDuration);

    std::vector<int> timeVec;
    std::vector<ArrayXd> frames;
    int t = 0;
    while (t < len - frameDuration) {
        timeVec.push_back(t + halfDur);

        ArrayXd frame = res.segment(t, frameDuration);
        frame *= win;
        frame -= frame.mean();

        frames.push_back(std::move(frame));

        t += shift;
    }

    const int N = timeVec.size();

    time = Map<ArrayXi>(timeVec.data(), N);

    // Create spectrogram matrix.
    std::vector<ArrayXd> specs(N);
    
    rfft_plan(fs);
    Map<ArrayXd> xin(rfft_in(fs), fs);
    Map<ArrayXd> xout(rfft_out(fs), fs);

    for (int i = 0; i < N; ++i) {
        xin.setZero();
        xin.head(frameDuration) = frames.at(i);

        rfft(fs);

        specs.at(i) = xout.head(fs / 2);
        specs.at(i) /= std::sqrt(specs.at(i).square().sum());
    }

    bool noPitchRangeAdjustment = true;
    ArrayXd srhVal(N);

    std::vector<int> f0s;
    
    for (int iter = 1; iter <= nIter; ++iter) {
        SRH(specs, nHarmonics, f0min, f0max, pitch, srhVal);
        
        if (srhVal.maxCoeff() > srhIterThresh) {
            // median of the F0s for which the SRH value is above srhIterThresh.
            f0s.clear();
            for (int iframe = 0; iframe < N; ++iframe) {
                if (srhVal(iframe) > srhIterThresh) {
                    f0s.push_back(pitch(iframe));
                }
            }
            std::nth_element(f0s.begin(), f0s.begin() + f0s.size() / 2, f0s.end());
            int f0medEst = f0s.at(f0s.size() / 2);

            // Only refine F0 limits if within the original limits.
            if (std::round(0.5 * f0medEst) > f0min) {
                f0min = std::round(0.5 * f0medEst);
                noPitchRangeAdjustment = false;
            }
            if (std::round(2.0 * f0medEst) < f0max) {
                f0max = std::round(2.0 * f0medEst);
                noPitchRangeAdjustment = false;
            }
        }

        if (noPitchRangeAdjustment) {
            break;
        }
    }

    // std(srhVal)
    double stdSrh = std::sqrt((srhVal - srhVal.mean()).square().sum() / (N - 1));

    if (stdSrh <= srhStdThresh) {
        voicing = (srhVal > voicingThresh).cast<int>();
    }
    else {
        voicing = (srhVal > voicingThresh2).cast<int>();
    } 

}

static void SRH(const std::vector<ArrayXd> & specs, int nHarmonics, int f0min, int f0max, ArrayXd & f0, ArrayXd & srhVal)
{
    const int N = specs.size();
    
    ArrayXd srh(f0max + 1);
    srh.head(f0min).setZero();

    f0.resize(N);
    srhVal.resize(N);

    for (int iframe = 0; iframe < N; ++iframe) {
        
        const ArrayXd & E = specs.at(iframe);

        for (int f = f0min; f <= f0max; ++f) {
            // SRH(f) = E(f) + sum(E(k * f) - E((k - .5) * f))
            
            srh(f) = E(f);

            for (int k = 2; k <= nHarmonics; ++k) {
                srh(f) += (E(k * f) - E(Index((k - 0.5) * f)));
            }
        }

        // f0 is the index with the highest SRH value.
        srhVal(iframe) = srh.maxCoeff(&f0(iframe));
    }
    
}
