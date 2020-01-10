#ifndef VC_AUDIO_AUDIOINTERFACE_H
#define VC_AUDIO_AUDIOINTERFACE_H

#include <QString>
#include <QList>
#include <atomic>
#include "miniaudio.h"

#include "../vc/VoiceChangerConfig.h"
#include "../vc/VoiceChanger.h"

struct AudioDeviceInfo
{
    ma_device_id id;
    QString name;
};

class AudioInterface : public QObject
{
    Q_OBJECT
public:
    AudioInterface();
    virtual ~AudioInterface();

    bool openFile(const QString & filePath);
    bool applyVoiceChangerToFile(const VoiceChangerConfig & config);
    void releaseAudioData();

    bool openPlaybackDevice(ma_device_id * deviceId);
    bool closePlaybackDevice();

    bool toggleVoiceChanger(bool toggle);
    bool seekTo(double time);

    bool play();
    bool pause();
    bool stop();

    const QList<AudioDeviceInfo> & getPlaybackDevices() const;
    bool isPlaying() const;
    double getCurrentTime() const;
    double getDuration() const;

signals:
    void fileLoaded(const QString & filePath);
    void started();
    void paused();
    void stopped();
    void seek(double time);
    void toggled(bool vc);

private:
    VoiceChanger voiceChanger;

    ma_context maCtx;

    QList<AudioDeviceInfo> devices;

    bool deviceOpen;
    ma_device device;

    std::atomic<bool> playing;
    std::atomic<bool> playOriginal;
    std::atomic<ma_uint64> frameIndex;

    float * original;
    float * modified;
    ma_uint64 frameCount;
    int sampleRate;

    static void maDataCallback(ma_device * pDevice, void * pOutput, const void * pInput, ma_uint32 frameCount);

};

#endif // VC_AUDIO_AUDIOINTERFACE_H
