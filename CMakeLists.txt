cmake_minimum_required(VERSION 3.5)

project(voice_changer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_FLAGS "-fopenmp -fdiagnostics-color -ftree-vectorize")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake_support)

find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(PortAudio REQUIRED)
find_package(Eigen3 REQUIRED NO_MODULE)
find_package(FFTW REQUIRED DOUBLE_LIB)
find_package(BLAS REQUIRED)

set(SOURCES
    resources.qrc
    src/main.cpp
    src/utils.h
    src/audio/dr_flac.h
    src/audio/dr_mp3.h
    src/audio/dr_wav.h
    src/audio/stb_vorbis.h
    src/audio/miniaudio.h
    src/audio/implementation.cpp
    src/audio/AudioInterface.h
    src/audio/AudioInterface.cpp
    src/vc/FFT/FFT.h
    src/vc/FFT/FFT.cpp
    src/vc/GlottalSource/McLeod/autocorrelation.cpp
    src/vc/GlottalSource/McLeod/parabolic_interpolation.cpp
    src/vc/GlottalSource/McLeod/peak_picking.cpp
    src/vc/GlottalSource/REAPER/core/file_resource.h
    src/vc/GlottalSource/REAPER/core/file_resource.cc
    src/vc/GlottalSource/REAPER/core/float_matrix-inl.h
    src/vc/GlottalSource/REAPER/core/float_matrix.h
    src/vc/GlottalSource/REAPER/core/float_matrix.cc
    src/vc/GlottalSource/REAPER/core/track.h
    src/vc/GlottalSource/REAPER/core/track.cc
    src/vc/GlottalSource/REAPER/epoch_tracker/epoch_tracker.h
    src/vc/GlottalSource/REAPER/epoch_tracker/epoch_tracker.cc
    src/vc/GlottalSource/REAPER/epoch_tracker/fd_filter.h
    src/vc/GlottalSource/REAPER/epoch_tracker/fd_filter.cc
    src/vc/GlottalSource/REAPER/epoch_tracker/fft.h
    src/vc/GlottalSource/REAPER/epoch_tracker/fft.cc
    src/vc/GlottalSource/REAPER/epoch_tracker/lpc_analyzer.h
    src/vc/GlottalSource/REAPER/epoch_tracker/lpc_analyzer.cc
    src/vc/GlottalSource/REAPER/REAPER.h
    src/vc/GlottalSource/REAPER/REAPER.cc
    src/vc/GlottalSource/EKF/EKF.h
    src/vc/GlottalSource/EKF/model.cpp
    src/vc/GlottalSource/EKF/optimise_Tp.cpp
    src/vc/GlottalSource/EKF/optimise_alpha.cpp
    src/vc/GlottalSource/EKF/optimise_epsilon.cpp
    src/vc/GlottalSource/Pitch.h
    src/vc/GlottalSource/Pitch_MPM.cpp
    src/vc/GlottalSource/Pitch_SWIPE.cpp
    src/vc/GlottalSource/Pitch.cpp
    src/vc/GlottalSource/LF.h
    src/vc/GlottalSource/lfEpsAlpha.cpp
    src/vc/GlottalSource/lfGfmSpec.cpp
    src/vc/GlottalSource/lfSynthFrame.cpp
    src/vc/GlottalSource/lfSynth.cpp
    src/vc/GlottalSource/Rd2tetpta.cpp
    src/vc/GlottalSource/Rd_msp.cpp
    src/vc/IAIF/IAIF.h
    src/vc/IAIF/IAIF.cpp
    src/vc/LPC/LPC.h
    src/vc/LPC/LPC.cpp
    src/vc/LPC/LPC_residual.cpp
    src/vc/Signal/Filter.h
    src/vc/Signal/Filter.cpp
    src/vc/Signal/Resample.h
    src/vc/Signal/Resample.cpp
    src/vc/Signal/Window.h
    src/vc/Signal/Window.cpp
    src/vc/Sinusoidal/Sin.h
    src/vc/Sinusoidal/Sin_analysis.cpp
    src/vc/Sinusoidal/Sin_shm.cpp
    src/vc/PitchShift/PS.h
    src/vc/PitchShift/PS.cpp
    src/vc/VoiceChangerConfig.h
    src/vc/VoiceChanger.h
    src/vc/VoiceChanger.cpp
    src/gui/MainWindow.h
    src/gui/MainWindow.cpp
)

add_executable(voice_changer WIN32 ${SOURCES})

include_directories(
    ${EIGEN_INCLUDE_DIR}
    ${PORTAUDIO_INCLUDE_DIRS}
    ${FFTW_INCLUDE_DIRS}
    ${Qt5Widgets_INCLUDE_DIRS}
)

target_link_libraries(voice_changer PUBLIC
    Eigen3::Eigen
    ${PORTAUDIO_LIBRARIES}
    ${FFTW_LIBRARIES}
    Qt5::Widgets
    ${BLAS_LIBRARIES}
)

if (UNIX AND NOT APPLE)
    target_link_libraries(voice_changer PUBLIC pthread m dl)
endif()

if (WIN32)
add_custom_command (TARGET speech_analysis POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt5::QWindowsIntegrationPlugin> $<TARGET_FILE_DIR:voice_changer>)
endif()

