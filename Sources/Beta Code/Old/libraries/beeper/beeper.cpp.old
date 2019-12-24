/* Beeper -- 
 *
 * Copyright 2016-2019 Baptiste PELLEGRIN
 * 
 * This file is part of GNUVario.
 *
 * GNUVario is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * GNUVario is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

/***************************************************************************************************/
/*                                BEEPER                                                           */
/*                                                                                                 */
/*  Ver     Date                                                                                   */
/*  1.0                                                                                            */
/*  1.1     30/08/19     Ajout message de debug                                                    */
/*  1.1.1   15/09/19     Ajout son de monté variable                                               */
/*                                                                                                 */
/***************************************************************************************************/

#include <Arduino.h>
#include <VarioSettings.h>
#include <beeper.h>

#include <DebugConfig.h>
#include <HardwareConfig.h>

Beeper beeper(volumeDefault);
 
/***************************************************************************************************/
Beeper::Beeper(uint8_t baseVolume) {
/***************************************************************************************************/

#ifdef SOUND_DEBUG
    SerialPort.print("Beeper - volume : ");
		SerialPort.println(baseVolume);
#endif //BUTTON_DEBUG


  /* save volume */
  volume = baseVolume;
  
  /* set threshold */
  setThresholds(BEEP_VELOCITY_DEFAULT_SINKING_THRESHOLD, BEEP_VELOCITY_DEFAULT_CLIMBING_THRESHOLD, BEEP_VELOCITY_DEFAULT_NEAR_CLIMBING_SENSITIVITY);
  
  /* init private vars */
  beepStartTime = 0;
  beepState = 0;
  beepType = BEEP_TYPE_SILENT;	
	
//	toneHAL.init();
}

/***************************************************************************************************/
Beeper::Beeper(uint32_t pin, uint8_t baseVolume) {
/***************************************************************************************************/
#ifdef SOUND_DEBUG
    SerialPort.print("Beeper - pin : ");
		SerialPort.println(pin);
    SerialPort.print("Beeper - volume : ");
		SerialPort.println(baseVolume);
#endif //BUTTON_DEBUG

  /* save volume */
  volume = baseVolume;
  
  /* set threshold */
  setThresholds(BEEP_VELOCITY_DEFAULT_SINKING_THRESHOLD, BEEP_VELOCITY_DEFAULT_CLIMBING_THRESHOLD, BEEP_VELOCITY_DEFAULT_NEAR_CLIMBING_SENSITIVITY);
  
  /* init private vars */
  beepStartTime = 0;
  beepState = 0;
  beepType = BEEP_TYPE_SILENT;	
	
//	toneHAL.init(pin);
}

/***************************************************************************************************/
void Beeper::init(double sinkingThreshold, double climbingThreshold, double nearClimbingSensitivity, uint8_t baseVolume) {
/***************************************************************************************************/
#ifdef SOUND_DEBUG
    SerialPort.println("Beeper : init");
#endif //BUTTON_DEBUG

  /* save volume */
  volume = baseVolume;
  
  /* set threshold */
  setThresholds(sinkingThreshold, climbingThreshold, nearClimbingSensitivity);
  
  /* init private vars */
  beepStartTime = 0;
  beepState = 0;
  beepType = BEEP_TYPE_SILENT;
}

/***************************************************************************************************/
void Beeper::setThresholds(double sinkingThreshold, double climbingThreshold, double nearClimbingSensitivity) {
/***************************************************************************************************/

  beepSinkingThreshold = sinkingThreshold;
  beepGlidingThreshold = climbingThreshold - nearClimbingSensitivity;
  beepClimbingThreshold = climbingThreshold;
}

/***************************************************************************************************/
void Beeper::setVolume(uint8_t newVolume) {
/***************************************************************************************************/
#ifdef SOUND_DEBUG
    SerialPort.print("Beeper - setvolume : ");
		SerialPort.println(newVolume);
#endif //BUTTON_DEBUG

  volume = newVolume;
  toneHAL.setVolume(newVolume);
}

/***************************************************************************************************/
uint8_t  Beeper::getVolume() {
/***************************************************************************************************/
  volume = toneHAL.getVolume();
  return volume;
}

/***************************************************************************************************/
void Beeper::setGlidingBeepState(boolean status) {
/***************************************************************************************************/
  if( status ) {
    bst_set(GLIDING_BEEP_ENABLED);
    if( beepType == BEEP_TYPE_GLIDING ) {
      beepStartTime = millis();
      beepPaternBasePosition = 0.0;
      beepPaternPosition = 0.0;
    }
  } else {
    bst_unset(GLIDING_BEEP_ENABLED);
  }
}
	    
/***************************************************************************************************/
void Beeper::setGlidingAlarmState(boolean status) {
/***************************************************************************************************/
  if( status ) {
    bst_set(GLIDING_ALARM_ENABLED);
  } else {
    bst_unset(GLIDING_ALARM_ENABLED);
  }
}


/***************************************************************************************************/
void Beeper::setBeepParameters(double velocity) {
/***************************************************************************************************/

  /* save velocity */
  beepVelocity = velocity;

  /* compute the beep freq that depend to beep type */
  switch( beepType ) {
  case BEEP_TYPE_SINKING :
    beepFreq = SINKING_BEEP_FREQ_COEFF * velocity + SINKING_BEEP_BASE_FREQ;
    break;

  case BEEP_TYPE_SILENT :
    beepFreq = 0.0;
    break;

  case BEEP_TYPE_GLIDING :
    beepFreq = CLIMBING_BEEP_FREQ_COEFF * velocity + CLIMBING_BEEP_BASE_FREQ;
    break;

  case BEEP_TYPE_CLIMBING :
    beepFreq = CLIMBING_BEEP_FREQ_COEFF * velocity + CLIMBING_BEEP_BASE_FREQ;
    break;
  }

}


/***************************************************************************************************/
void Beeper::setVelocity(double velocity) {
/***************************************************************************************************/
  
#ifdef SOUND_DEBUG
    SerialPort.print("Beeper - setvelocity : ");
		SerialPort.println(velocity);
#endif //BUTTON_DEBUG
	
  /* check if we need to change the beep type */
  boolean beepTypeChange = false;
  switch( beepType ) {
  case BEEP_TYPE_SINKING :
	
#ifdef SOUND_DEBUG
    SerialPort.println("Beeper - BEEP_TYPE_SINKING");
#endif //BUTTON_DEBUG
		
    if( velocity >  beepSinkingThreshold + BEEP_VELOCITY_SENSITIVITY )
      beepTypeChange = true;
    break;

  case BEEP_TYPE_SILENT :
#ifdef SOUND_DEBUG
    SerialPort.println("Beeper - BEEP_TYPE_SILENT");
#endif //SOUND_DEBUGG

    if( velocity < beepSinkingThreshold || velocity > beepGlidingThreshold )
      beepTypeChange = true;
    break;

  case BEEP_TYPE_GLIDING :
#ifdef SOUND_DEBUG
    SerialPort.println("Beeper - BEEP_TYPE_GLIDING");
#endif //SOUND_DEBUGG

    if( velocity < beepGlidingThreshold - BEEP_VELOCITY_SENSITIVITY || velocity > beepClimbingThreshold )
      beepTypeChange = true;
    break;

  case BEEP_TYPE_CLIMBING :
#ifdef SOUND_DEBUG
    SerialPort.println("Beeper - BEEP_TYPE_CLIMBING");
#endif //SOUND_DEBUGG

    if( velocity < beepClimbingThreshold - BEEP_VELOCITY_SENSITIVITY )
       beepTypeChange = true;
    break;
  }

  /* check if alarm need to be started */
  boolean startAlarm = false;
  if( bst_isset(GLIDING_ALARM_ENABLED) && beepTypeChange && !bst_isset(CLIMBING_ALARM) && !bst_isset(SINKING_ALARM) ) {
    /* need climbing alarm ? */
    if( beepType == BEEP_TYPE_SINKING || beepType == BEEP_TYPE_SILENT ) {
      if( velocity > beepGlidingThreshold && velocity < beepClimbingThreshold ) {
				startAlarm = true;
				bst_set(CLIMBING_ALARM);
      }
    }
    /* else need sinking alarm ? */
    else {
      if( velocity > beepSinkingThreshold && velocity < beepGlidingThreshold ) {
				startAlarm = true;
				bst_set(SINKING_ALARM);
      }
    }
  }

  /* check if alarm need to be stopped */
  /* (when climbing or sinking beep start ) */
  if( ( beepTypeChange ) &&
      ( bst_isset(CLIMBING_ALARM) || bst_isset(SINKING_ALARM) ) &&
      ( velocity > beepClimbingThreshold || velocity < beepSinkingThreshold ) ) {
    bst_unset(CLIMBING_ALARM);
    bst_unset(SINKING_ALARM);
  }
      
	

  /* start a new beep if needed */
  if( (beepTypeChange && !bst_isset(CLIMBING_ALARM) && !bst_isset(SINKING_ALARM) ) ||
      (startAlarm) ) {
    beepStartTime = millis();
		beepFreqUpdatePosition = beepStartTime;
    beepPaternBasePosition = 0.0;
    beepPaternPosition = 0.0;
    bst_set(BEEP_NEW_FREQ); //force changing freq
  }

  /* set the new beep type if changed */
  if( beepTypeChange ) {
    if( velocity < beepSinkingThreshold ) {
      beepType = BEEP_TYPE_SINKING;
    } else if( velocity < beepGlidingThreshold ) {
      beepType = BEEP_TYPE_SILENT;
    } else if( velocity < beepClimbingThreshold ) {
      beepType = BEEP_TYPE_GLIDING;
    } else {
      beepType = BEEP_TYPE_CLIMBING;
    }
  }
 
  /* check if we need to change the beep parameters  */
  /* !!! not forcing freq change on patern beeps !!! */
  /*     just wait for the next silent               */
  if( startAlarm || beepTypeChange ||
      velocity > beepVelocity + BEEP_VELOCITY_SENSITIVITY ||
      velocity < beepVelocity - BEEP_VELOCITY_SENSITIVITY ) {
    setBeepParameters(velocity);
    if( beepType == BEEP_TYPE_SINKING )
      bst_set(BEEP_NEW_FREQ);  //sinking beep don't have patern
  }
}

/***************************************************************************************************/
void Beeper::setBeepPaternPosition(double velocity) {
/***************************************************************************************************/

  /* check alarm */
  boolean haveAlarm = false;
  if( bst_isset(CLIMBING_ALARM) || bst_isset(SINKING_ALARM) ) {
    haveAlarm = true;
  }

  /***********************************/
  /* check if the beep have a patern */
  /***********************************/
  if( !haveAlarm &&
      (beepType == BEEP_TYPE_SINKING || beepType == BEEP_TYPE_SILENT) ) {
    return;
  }
  
  unsigned long currentTime = millis();
  double currentLength = (double)(currentTime - beepStartTime) / 1000.0;

  /*******************************************/
  /* does the position depends on velocity ? */
  /*******************************************/
  if( !haveAlarm &&
      beepType == BEEP_TYPE_CLIMBING ) {
    currentLength *= (beepVelocity * CLIMBING_BEEP_VELOCITY_FILTER_COEFF + CLIMBING_BEEP_VELOCITY_FILTER_BASE);

    /* avoid going backward */
    if( currentLength + beepPaternBasePosition > beepPaternPosition ) {
      beepPaternPosition = currentLength + beepPaternBasePosition;
    }
    if (currentTime > beepFreqUpdatePosition + CLIMBING_BEEP_FREQ_UPDATE && CLIMBING_BEEP_FREQ_UPDATE > 0) {
      bst_set(BEEP_NEW_FREQ);
      beepFreqUpdatePosition = currentTime;	
		}					
  } else {
    beepPaternPosition = currentLength;
  }

  /**************************************/
  /* check if the patern end is reached */
  /**************************************/

  /* alarm case */
  if( haveAlarm ) {
    /* climbing alarm */
    if( bst_isset(CLIMBING_ALARM) ) {
      /* if alarm done, reset */
      if( beepPaternPosition > CLIMBING_ALARM_LENGTH ) {
	bst_unset(CLIMBING_ALARM);
	beepStartTime = currentTime;
	beepPaternBasePosition = 0.0;
	beepPaternPosition = 0.0;
	setBeepPaternPosition(velocity);
	bst_set(BEEP_NEW_FREQ);
	return;
      }
    }
    /* sinking alarm */
    else {
      /* if alarm done reset */
      if( beepPaternPosition > SINKING_ALARM_LENGTH ) {
	bst_unset(SINKING_ALARM);
	beepStartTime = currentTime;
	beepPaternBasePosition = 0.0;
	beepPaternPosition = 0.0;
	setBeepPaternPosition(velocity);
	bst_set(BEEP_NEW_FREQ);
	return;
      }
    }
  }
  /* looping patern case */
  else {
    double loopingPaternLength;
    if(  beepType == BEEP_TYPE_GLIDING ) {
      loopingPaternLength = GLIDING_BEEP_LENGTH;
    }else {
      loopingPaternLength = CLIMBING_BEEP_LENGTH;
    }

    while( beepPaternPosition > loopingPaternLength ) {
        beepPaternPosition -= loopingPaternLength;
        beepStartTime = millis();
        beepPaternBasePosition = beepPaternPosition;
    }
  }
}


/***************************************************************************************************/
void Beeper::setTone() {
/***************************************************************************************************/
  
  /* alarme case */
  if(  bst_isset(CLIMBING_ALARM) || bst_isset(SINKING_ALARM) ) { 

    /******************/
    /* climbing alarm */
    /******************/
    if( bst_isset(CLIMBING_ALARM) ) {
#ifdef SOUND_DEBUG
			SerialPort.println("Beeper - setTone : CLIMBING_ALARM");
#endif //SOUND_DEBUGG

      /* get half position */
      double halfPaternPosition = beepPaternPosition;
      if( halfPaternPosition > (CLIMBING_ALARM_LENGTH/2.0) ) {
				halfPaternPosition -= (CLIMBING_ALARM_LENGTH/2.0);
      }

      /* set tone */
      if( halfPaternPosition < CLIMBING_ALARM_HIGH_LENGTH ) {
				if( !bst_isset(BEEP_HIGH) ) {
					toneHAL.tone(CLIMBING_ALARM_FREQ, volume);					
					bst_set(BEEP_HIGH);
				} else if( bst_isset(BEEP_NEW_FREQ) ) {
					toneHAL.tone(CLIMBING_ALARM_FREQ, volume);
				}
      } else {
				toneHAL.tone(0.0);
				bst_unset(BEEP_HIGH);
      }
    }

    /*****************/
    /* sinking alarm */
    /*****************/
    else {
#ifdef SOUND_DEBUG
			SerialPort.println("Beeper - setTone : SINKING_ALARM");
#endif //SOUND_DEBUGG
			
      if( !bst_isset(BEEP_HIGH) || bst_isset(BEEP_NEW_FREQ) ) {
				toneHAL.tone(SINKING_ALARM_FREQ, volume);
				bst_set(BEEP_HIGH);
      }
    }
  } else {
    
    /****************/
    /* sinking beep */
    /****************/
    if( beepType == BEEP_TYPE_SINKING ) {
#ifdef SOUND_DEBUG
			SerialPort.println("Beeper - setTone : BEEP_TYPE_SINKING");
#endif //SOUND_DEBUGG
 
			if( !bst_isset(BEEP_HIGH) || bst_isset(BEEP_NEW_FREQ) ) {
				toneHAL.tone(beepFreq, volume);
				bst_set(BEEP_HIGH);
      }
    }

    /**********/
    /* silent */
    /**********/
    else if( beepType == BEEP_TYPE_SILENT ) {
#ifdef SOUND_DEBUG
		 SerialPort.println("Beeper - setTone : BEEP_TYPE_SILENT");
#endif //SOUND_DEBUGG
     toneHAL.tone(0.0);
      bst_unset(BEEP_HIGH);
    }

    /***********/
    /* gliding */
    /***********/
    else if(  beepType == BEEP_TYPE_GLIDING ) {
#ifdef SOUND_DEBUG
		 SerialPort.println("Beeper - setTone : BEEP_TYPE_GLIDING");
#endif //SOUND_DEBUGG
     if( bst_isset(GLIDING_BEEP_ENABLED) ) {
				if( beepPaternPosition < GLIDING_BEEP_HIGH_LENGTH ) {
					if( !bst_isset(BEEP_HIGH) ) {
						toneHAL.tone(beepFreq, volume);
						bst_set(BEEP_HIGH);
					} else if( bst_isset(BEEP_NEW_FREQ) ) {
						toneHAL.tone(beepFreq, volume);
					}
				} else {
					toneHAL.tone(0.0);
					bst_unset(BEEP_HIGH);
				}
      } else {
				toneHAL.tone(0.0);
				bst_unset(BEEP_HIGH);
      }
    }

    /************/
    /* climbing */
    /************/
    else {
#ifdef SOUND_DEBUG
		 SerialPort.println("Beeper - setTone : BEEP_TYPE_CLIMBING");
#endif //SOUND_DEBUGG
       if( beepPaternPosition < CLIMBING_BEEP_HIGH_LENGTH ) {
				if( !bst_isset(BEEP_HIGH) ) {
#ifdef SOUND_DEBUG
					SerialPort.println("Beeper - setTone : BEEP_TYPE_CLIMBING / BEEP_HIGH");
#endif //SOUND_DEBUGG
 					
					toneHAL.tone(beepFreq, volume);
					bst_set(BEEP_HIGH);
				} else if( bst_isset(BEEP_NEW_FREQ) ) {
#ifdef SOUND_DEBUG
					SerialPort.println("Beeper - setTone : BEEP_TYPE_CLIMBING / BEEP_NEW_FREQ");
#endif //SOUND_DEBUGG
 					toneHAL.tone(beepFreq, volume);
					bst_set(BEEP_HIGH);
			 } else {
#ifdef SOUND_DEBUG
					SerialPort.println("Beeper - setTone : BEEP_TYPE_CLIMBING / CLIMBING_BEEP_HIGH_LENGTH");
#endif //SOUND_DEBUGG				 
//					toneHAL.tone(beepFreq, volume);
			 }
      } else {
#ifdef SOUND_DEBUG
					SerialPort.println("Beeper - setTone : BEEP_TYPE_CLIMBING / BEEP_LOW");
#endif //SOUND_DEBUGG
 				toneHAL.tone(0.0);
				bst_unset(BEEP_HIGH);
      }
    }
  }

  /***************/
  /* tone is set */
  /***************/
  bst_unset(BEEP_NEW_FREQ);
}	


/***************************************************************************************************/
void Beeper::update() {
/***************************************************************************************************/
  setBeepPaternPosition(beepVelocity);
  setTone();
  
}


/***************************************************************************************************/
void Beeper::setFrequency(uint32_t fHz) {
/***************************************************************************************************/
	if (fHz ) {
    toneHAL.tone(fHz, volume);   //volume);
	}
	else {
		toneHAL.noTone();
	}
}


/***************************************************************************************************/
void Beeper::generateTone(uint32_t fHz, int ms) {
/***************************************************************************************************/
    setFrequency(fHz);
    delay(ms);
    setFrequency(0);
}

/***************************************************************************************************/
void Beeper::tone(uint32_t fHz) {
/***************************************************************************************************/
	setFrequency(fHz);
}

/***************************************************************************************************/
void Beeper::noTone(void) {
/***************************************************************************************************/
	toneHAL.noTone();
}
