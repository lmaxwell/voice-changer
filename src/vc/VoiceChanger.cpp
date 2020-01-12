#include <Eigen/Dense>
#include "VoiceChanger.h"
#include "Signal/Filter.h"
#include "Signal/Resample.h"
#include "Signal/Window.h"
#include "GlottalSource/Pitch.h"
#include "Sinusoidal/Sin.h"
#include "GlottalSource/LF.h"

using namespace Eigen;

VoiceChanger::VoiceChanger()
    : config()
{
}

void VoiceChanger::setConfig(const VoiceChangerConfig & _config)
{
    config = _config;
}

bool VoiceChanger::convert(const float * _input, float * _output, ma_uint64 frameCount, int sampleRate)
{
    Map<const ArrayXf> inputf(_input, frameCount);
    Map<ArrayXf> outputf(_output, frameCount);

    ArrayXd input = inputf.cast<double>();
    ArrayXd output;

    // do stuff

    // Pitch analysis.
    ArrayXd pitch;
    ArrayXi voicing;
    ArrayXi time;
    Pitch::estimate(input, sampleRate, 60.0, 3000.0, 10, pitch, voicing, time);

    // Sinusoidal analysis.
    for (int i = 0; i < time.size(); ++i) {
        if (!voicing(i))
            pitch(i) = 100.0;
    }

    std::vector<SinFrame> sinFrames;
    sinAnalysis(input, sampleRate, pitch, time, sinFrames);
    
    // LF analysis.
    for (int i = 0; i < time.size(); ++i) {
        if (!voicing(i))
            sinFrames.at(i).f0 = 0;
    }    
    std::vector<LF_Frame> lfFrames;
    Rd_msp(sinFrames, sampleRate, lfFrames);

    // LF synthesis.
    output.setZero(frameCount);
    lfSynth(lfFrames, voicing, time, sampleRate, output);

    // end do stuff

    outputf = output.cast<float>();

    return true;
}
