#include <Eigen/Dense>
#include "VoiceChanger.h"
#include "Signal/Filter.h"
#include "Signal/Resample.h"
#include "Signal/Window.h"
#include "IAIF/IAIF.h"
#include "PitchShift/PS.h"

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

    IAIF_Result r;
    iaif(
        r,
        input,
        sampleRate,
        25.0,
        5.0,
        sampleRate / 1000.0 + 2,
        4,
        0.997
    );
  
    output = r.g;

    // end do stuff

    outputf = output.cast<float>();

    return true;
}
