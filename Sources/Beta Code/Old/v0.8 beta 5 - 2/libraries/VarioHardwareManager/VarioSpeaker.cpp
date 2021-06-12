#include <Arduino.h>
#include <HardwareConfig.h>
#include <DebugConfig.h>
#include "VarioSpeaker.h"

#include <VarioSettings.h>

VarioSpeaker::VarioSpeaker() {}

//*************************
void VarioSpeaker::init()
//*************************
{
#if defined(HAVE_SPEAKER)
    toneHAL.init();
    beeper.setVolume(10);
    //beeper.generateTone(2000,300);
#endif
}

//***********************************************
// INIT Sound
//      init Beeper avec les valeurs personnelles
//      init Volume
//***********************************************

//*************************
void VarioSpeaker::initSound()
//*************************
{
#ifdef HAVE_SPEAKER
  beeper.init(GnuSettings.VARIOMETER_SINKING_THRESHOLD, GnuSettings.VARIOMETER_CLIMBING_THRESHOLD, GnuSettings.VARIOMETER_NEAR_CLIMBING_SENSITIVITY);

  GnuSettings.VARIOMETER_BEEP_VOLUME = GnuSettings.soundSettingRead();
  beeper.setVolume(GnuSettings.VARIOMETER_BEEP_VOLUME);
  toneHAL.setVolume(GnuSettings.VARIOMETER_BEEP_VOLUME);

#ifdef SOUND_DEBUG
  SerialPort.print("Volume SOUND : ");
  SerialPort.println(GnuSettings.VARIOMETER_BEEP_VOLUME);
#endif //SOUND_DEBUG

#endif //HAVE_SPEAKER	
}