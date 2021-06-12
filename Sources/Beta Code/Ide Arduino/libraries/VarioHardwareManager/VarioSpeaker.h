
#ifndef VARIO_SPEAKER_H
#define VARIO_SPEAKER_H

#include <HardwareConfig.h>

#ifdef HAVE_SPEAKER
#include <toneHAL.h>
#include <beeper.h>
#endif //HAVE_SPEAKER

class VarioSpeaker
{
public:
    VarioSpeaker();
    void init();
		void initSound();
		void UnMute(void);
		void Mute(void);
};

#endif //VARIO_SPEAKER_H