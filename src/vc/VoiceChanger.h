#ifndef VC_VC_VOICECHANGER_H
#define VC_VC_VOICECHANGER_H

#include "../audio/miniaudio.h"
#include "VoiceChangerConfig.h"

class VoiceChanger
{
public:
    VoiceChanger();

    void setConfig(const VoiceChangerConfig & config);
    bool convert(const float * input, float * output, ma_uint64 frameCount, int sampleRate);

private:
    VoiceChangerConfig config;
};

#endif // VC_VC_VOICECHANGER_H
