#include <cmath>
#include <iostream>
#include "AudioInterface.h"

AudioInterface::AudioInterface()
    : voiceChanger(),
      deviceOpen(false),
      playing(false),
      playOriginal(true),
      frameIndex(0),
      original(nullptr),
      modified(nullptr),
      frameCount(0),
      sampleRate(48000)
{
    ma_result result;
    
    result = ma_context_init(nullptr, 0, nullptr, &maCtx);
    if (result != MA_SUCCESS) {
        throw std::runtime_error("Could not initialise miniaudio context.");
    }

    ma_device_info * deviceInfos;
    ma_uint32 deviceCount;

    result = ma_context_get_devices(&maCtx, &deviceInfos, &deviceCount, nullptr, nullptr);
    if (result != MA_SUCCESS) {
        throw std::runtime_error("Could not enumerate playback devices.");
    }

    devices.reserve(deviceCount);
    for (ma_uint32 deviceIndex = 0; deviceIndex < deviceCount; ++deviceIndex)
    {
        const auto & info = deviceInfos[deviceIndex];

        devices.append(AudioDeviceInfo{
            .id = info.id,
            .name = QString::fromUtf8(info.name),
        });
    }
}

AudioInterface::~AudioInterface()
{
    ma_result result;
    
    result = ma_context_uninit(&maCtx);
    if (result != MA_SUCCESS) {
        std::cerr << "Could not uninitialise miniaudio context." << std::endl;
    }
}

bool AudioInterface::openFile(const QString & filePath)
{
    if (original != nullptr) {
        return false;
    }

    ma_decoder_config config;
    ma_result result;

    config = ma_decoder_config_init(ma_format_f32, 1, sampleRate);
    result = ma_decode_file(qPrintable(filePath), &config, &frameCount, (void **) &original);
    sampleRate = config.sampleRate;
    if (result != MA_SUCCESS) {
        return false;
    }

    emit fileLoaded(filePath);
    toggleVoiceChanger(false);

    return true;
}

bool AudioInterface::applyVoiceChangerToFile(const VoiceChangerConfig & config)
{
    if (original == nullptr) {
        return false;
    }
 
    voiceChanger.setConfig(config);

    modified = (float *) ma_malloc(frameCount * sizeof(float));
    if (!voiceChanger.convert(original, modified, frameCount, sampleRate)) {
        ma_free(modified);
        modified = nullptr;
        return false;
    }

    return true;
}

void AudioInterface::releaseAudioData()
{
    playing.store(false);
    playOriginal.store(true);
    frameIndex.store(0);

    if (original != nullptr) {
        ma_free(original);
    }
    if (modified != nullptr) {
        ma_free(modified);
    }
    
    original = nullptr;
    modified = nullptr;
    frameCount = 0;
    sampleRate = 48000;
}

const QList<AudioDeviceInfo> & AudioInterface::getPlaybackDevices() const
{
    return devices;
}

bool AudioInterface::openPlaybackDevice(ma_device_id * deviceId)
{
    if (deviceOpen) {
        return false;
    }

    ma_result result;
    ma_device_config config;

    config = ma_device_config_init(ma_device_type_playback);
    config.sampleRate = sampleRate;
    config.noClip = true;
    config.playback.format = ma_format_f32;
    config.playback.channels = 1;
    config.playback.pDeviceID = deviceId;
    config.dataCallback = &AudioInterface::maDataCallback;
    config.pUserData = this;

    result = ma_device_init(&maCtx, &config, &device);
    if (result != MA_SUCCESS) {
        return false;
    }

    result = ma_device_start(&device);
    if (result != MA_SUCCESS) {
        ma_device_uninit(&device);
        return false;
    }

    deviceOpen = true;

    return true;
}

bool AudioInterface::closePlaybackDevice()
{
    if (!deviceOpen) {
        return false;
    }

    ma_device_uninit(&device);
    deviceOpen = false;

    return true;
}

bool AudioInterface::toggleVoiceChanger(bool toggle)
{
    if (toggle && modified == nullptr) {
        return false;
    }

    // toggle voice changer
    playOriginal.store(!toggle);

    emit toggled(toggle);

    return true;
}

bool AudioInterface::seekTo(double time)
{
    bool playOriginal = this->playOriginal.load();
    if ((playOriginal && original == nullptr) || (!playOriginal && modified == nullptr)) {
        return false;
    }

    frameIndex.store(std::floor(time * sampleRate));
    emit seek(time);

    return true;
}

bool AudioInterface::play()
{
    if (!deviceOpen || (playOriginal && original == nullptr) || (!playOriginal && modified == nullptr)) {
        return false;
    }

    playing.store(true);
    emit started();

    return true;
}

bool AudioInterface::pause()
{
    if (!deviceOpen || (playOriginal && original == nullptr) || (!playOriginal && modified == nullptr)) {
        return false;
    }
 
    playing.store(false);
    emit paused();

    return true;
}

bool AudioInterface::stop()
{
    if (!deviceOpen || (playOriginal && original == nullptr) || (!playOriginal && modified == nullptr)) {
        return false;
    }

    playing.store(false);
    seekTo(0.0);
    emit stopped();

    return true;
}

bool AudioInterface::isPlaying() const
{
    return playing.load();
}

double AudioInterface::getCurrentTime() const
{
    bool playOriginal = this->playOriginal.load();
    if ((playOriginal && original == nullptr) || (!playOriginal && modified == nullptr)) {
        return -1.0;
    }

    return (double) frameIndex.load() / (double) sampleRate;
}

double AudioInterface::getDuration() const
{
    if (original == nullptr) {
        return -1.0;
    }
    else {
        return (double) frameCount / (double) sampleRate;
    }
}

void AudioInterface::maDataCallback(ma_device * pDevice, void * pOutput, const void * pInput, ma_uint32 frameCount)
{
    auto that = (AudioInterface *) pDevice->pUserData;
    if (that == nullptr) {
        return;
    }

    if (!that->playing.load()) {
        return;
    }

    ma_uint64 audioFrameCount = that->frameCount;
    ma_uint64 audioFrameIndex = that->frameIndex.load();
    float * audioData = that->playOriginal.load() ? that->original : that->modified;

    if (audioFrameIndex >= audioFrameCount) {
        that->stop();
        return;
    }

    ma_uint32 copyCount = std::min(frameCount, (ma_uint32) (audioFrameCount - audioFrameIndex));
    memcpy(pOutput, audioData + audioFrameIndex, copyCount * sizeof(float));
    that->frameIndex += copyCount;
    
    double time = (double) audioFrameIndex / (double) that->sampleRate;
    emit that->seek(time);
}
