#ifndef VC_GUI_MAINWINDOW_H
#define VC_GUI_MAINWINDOW_H

#include <QtWidgets>
#include "../audio/AudioInterface.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow();

protected:
    void closeEvent(QCloseEvent * event);

private slots:
    void updateVoiceChangedAudio();
    void chooseAudioFile();
    void toggleVoiceChanger(bool value);
    void togglePlayback();
    void stopPlayback();
    void seek(int value);

private:
    void updateSeekLabel(int current, int total);

    AudioInterface audio;
    VoiceChangerConfig vc;

    QPushButton * chooseButton;
    QLabel * fileLabel;
    
    QCheckBox * toggleVoice;
    QPushButton * playPauseButton;
    QPushButton * stopButton;
    QSlider * seekSlider;
    QLabel * seekLabel;

    QPushButton * renderButton;
};

#endif // VC_GUI_MAINWINDOW_H
