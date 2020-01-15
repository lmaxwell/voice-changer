#ifndef VC_VC_REAPER_REAPER_H
#define VC_VC_REAPER_REAPER_H

#include "core/track.h"
#include "epoch_tracker/epoch_tracker.h"

Track* MakeEpochOutput(EpochTracker &et, float unvoiced_pm_interval);
Track* MakeF0Output(EpochTracker &et, float resample_interval, Track** cor);

bool ComputeEpochsAndF0(EpochTracker &et, float unvoiced_pulse_interval,
			float external_frame_interval,
			Track** pm, Track** f0, Track** corr);

#endif // VC_VC_REAPER_REAPER_H
