#include "MainWindow.h"

MainWindow::MainWindow()
{
    setWindowTitle("Clo's voice changer");

    auto central = new QWidget;
    setCentralWidget(central);

    auto lyMain = new QVBoxLayout(central);

    auto wAudio = new QWidget(central);
    auto lyAudio = new QVBoxLayout(wAudio);
    wAudio->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);
    {
        // Audio file selection.
        auto w1 = new QWidget;
        auto ly1 = new QHBoxLayout(w1);
        w1->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
        {
            chooseButton = new QPushButton("Choose audio file");

            connect(chooseButton, &QAbstractButton::clicked, this, &MainWindow::chooseAudioFile);

            fileLabel = new QLabel("No file selected");

            ly1->addWidget(chooseButton, 0, Qt::AlignHCenter);
            ly1->addWidget(fileLabel, 0, Qt::AlignHCenter);
        }
        lyAudio->addWidget(w1, 0, Qt::AlignCenter);
       
        // Playback controls.
        auto w2 = new QWidget;
        auto ly2 = new QHBoxLayout(w2);
        w2->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
        {
            toggleVoice = new QCheckBox("Toggle voice changer");
            toggleVoice->setChecked(false);
            
            connect(toggleVoice, &QAbstractButton::toggled, this, &MainWindow::toggleVoiceChanger);
            playPauseButton = new QPushButton;
            playPauseButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
            playPauseButton->setEnabled(false);

            connect(playPauseButton, &QAbstractButton::clicked, this, &MainWindow::togglePlayback);

            stopButton = new QPushButton;
            stopButton->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
            stopButton->setEnabled(false);

            connect(stopButton, &QAbstractButton::clicked, this, &MainWindow::stopPlayback);

            seekSlider = new QSlider(Qt::Horizontal);
            seekSlider->setRange(0, 0);
            seekSlider->setEnabled(false);

            connect(seekSlider, &QAbstractSlider::sliderMoved, this, &MainWindow::seek);

            seekLabel = new QLabel("--:-- / --:--");

            ly2->addWidget(toggleVoice, 0, Qt::AlignHCenter);
            ly2->addWidget(playPauseButton, 0, Qt::AlignHCenter);
            ly2->addWidget(stopButton, 0, Qt::AlignHCenter);
            ly2->addWidget(seekSlider, 0, Qt::AlignHCenter);
            ly2->addWidget(seekLabel, 0, Qt::AlignHCenter);
        }
        lyAudio->addWidget(w2, 0, Qt::AlignCenter);
    }

    auto wVoice = new QWidget(central);
    auto lyVoice = new QVBoxLayout(wVoice);
    wVoice->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    {
        // Presets and render.
        auto w1 = new QWidget;
        auto ly1 = new QHBoxLayout(w1);
        w1->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
        {
            renderButton = new QPushButton("Render");
            renderButton->setEnabled(false);
            
            connect(renderButton, &QAbstractButton::clicked, this, &MainWindow::updateVoiceChangedAudio);

            ly1->addWidget(renderButton, 0, Qt::AlignRight);
        }
        lyVoice->addWidget(w1, 0, Qt::AlignCenter);
    }
    
    lyMain->addWidget(wAudio, 0, Qt::AlignTop);
    lyMain->addWidget(wVoice, 0, Qt::AlignCenter);
   
    connect(&audio, &AudioInterface::fileLoaded, [&](const QString & filePath) {
        fileLabel->setText(QFileInfo(filePath).fileName());

        playPauseButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
        playPauseButton->setEnabled(true);
        stopButton->setEnabled(true);
        seekSlider->setEnabled(true);

        double duration = audio.getDuration();
        seekSlider->setRange(0, std::floor(100.0 * duration));
        updateSeekLabel(0, duration);

        renderButton->setEnabled(true);
    });

    connect(&audio, &AudioInterface::started, [&]() {
        playPauseButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
    });

    connect(&audio, &AudioInterface::paused, [&]() {
        playPauseButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    });

    connect(&audio, &AudioInterface::stopped, [&]() {
        playPauseButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    });

    connect(&audio, &AudioInterface::seek, [&](double time) {
        int value = std::floor(100.0 * time);
        double duration = audio.getDuration();
        seekSlider->setValue(std::floor(100.0 * time));
        updateSeekLabel(time, duration);
    });

    connect(&audio, &AudioInterface::toggled, [&](bool vc) {
        renderButton->setEnabled(!vc);
        if (vc) {
            renderButton->setToolTip("Cannot render while voice changer playback is enabled.");
        }
        else {
            renderButton->setToolTip("");
        }
    });

    audio.openPlaybackDevice(nullptr);
}

void MainWindow::closeEvent(QCloseEvent * event)
{
    audio.releaseAudioData();
}

void MainWindow::updateVoiceChangedAudio()
{
    if (!audio.applyVoiceChangerToFile(vc)) {
        QMessageBox::warning(this, "Could not render audio", "Unable to render audio. Try again.");
    }
}

void MainWindow::chooseAudioFile()
{
    QString filePath = QFileDialog::getOpenFileName(
        this,
        "Open audio file",
        QDir::currentPath(),
        "Audio files (*.flac, *.mp3, *.wav, *.ogg) (*.flac *.mp3 *.wav *.ogg)",
        nullptr,
        QFileDialog::HideNameFilterDetails
    );
    
    if (filePath.isEmpty()) {
        return;
    }

    audio.releaseAudioData();

    if (!audio.openFile(filePath)) {
        QMessageBox::warning(this, "Could not open audio file", "Unable to open audio file. Try again.");
        fileLabel->setText("No file selected");
        playPauseButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
        playPauseButton->setEnabled(false);
        stopButton->setEnabled(false);
        seekSlider->setEnabled(false);
        seekSlider->setRange(0, 0);
        seekSlider->setValue(0);
        updateSeekLabel(-1, -1);
    }
}

void MainWindow::toggleVoiceChanger(bool value)
{
    if (!audio.toggleVoiceChanger(value)) {
        QMessageBox::warning(this, "Could not toggle voice changer", "Unable to toggle voice changer. Try again.");
        toggleVoice->setChecked(false);
        renderButton->setEnabled(true);
    }
}

void MainWindow::togglePlayback()
{
    if (audio.isPlaying() && !audio.pause()) {
        QMessageBox::warning(this, "Could not pause playback", "Unable to pause playback. Try again.");
    }
    else if (!audio.isPlaying() && !audio.play()) {
        QMessageBox::warning(this, "Could not start playback", "Unable to start playback. Try again.");
    }
}

void MainWindow::stopPlayback()
{
    if (audio.isPlaying() && !audio.stop()) {
        QMessageBox::warning(this, "Could not stop playback", "Unable to stop playback. Try again.");
    }
}

void MainWindow::seek(int value)
{
    if (value >= 0) {
        double time = (double) value / 100.0;

        if (!audio.seekTo(time)) {
            QMessageBox::warning(this, "Could not seek audio", "Unable to seek audio. Try again.");
        }
    }
    else {
        updateSeekLabel(-1, -1);
    }
}

void MainWindow::updateSeekLabel(int current, int total)
{ 
    QTime zero(0, 0);

    QString currentString = (current >= 0) ? zero.addSecs(current).toString("mm:ss") : "--:--";
    QString totalString = (total >= 0) ? zero.addSecs(total).toString("mm:ss") : "--:--";

    seekLabel->setText(QString("%1 / %2").arg(currentString, totalString));
}
