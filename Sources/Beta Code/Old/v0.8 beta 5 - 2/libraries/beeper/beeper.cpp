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
/*  1.2     01/11/19     Suppression de la gestion du volume dans beeper - le volume ne sera géré  */
/*                       que dans toneHAL                                                          */
/*  1.3      13/09/20    Ajout VarioXBeeper                                                        */
/*  1.3.1    17/09/20    Ajout duty / cycle variable et parametrable                               */
/*  1.3.2    02/10/20    Calcul en milliseconde                                                    */
/*                                                                                                 */
/***************************************************************************************************/

#include <Arduino.h>
#include <VarioSettings.h>
#include <beeper.h>

#include <DebugConfig.h>
#include <HardwareConfig.h>

#include <VarioXBeeper.h>

#define BEEPERVARIABLE

Beeper beeper(volumeDefault);
 
/***************************************************************************************************/
Beeper::Beeper(uint8_t baseVolume) {
/***************************************************************************************************/

/*
#ifdef SOUND_DEBUG
    SerialPort.print("Beeper - volume : ");
		SerialPort.println(baseVolume);
#endif //BUTTON_DEBUG


  // save volume 
  //volume = baseVolume;
  
  // set threshold 
  setThresholds(BEEP_VELOCITY_DEFAULT_SINKING_THRESHOLD, BEEP_VELOCITY_DEFAULT_CLIMBING_THRESHOLD, BEEP_VELOCITY_DEFAULT_NEAR_CLIMBING_SENSITIVITY);
  
  // init private vars 
  beepStartTime = 0;
  beepState = 0;
  beepType = BEEP_TYPE_SILENT;	
	
//	toneHAL.init();*/

  init(BEEP_VELOCITY_DEFAULT_SINKING_THRESHOLD, BEEP_VELOCITY_DEFAULT_CLIMBING_THRESHOLD, BEEP_VELOCITY_DEFAULT_NEAR_CLIMBING_SENSITIVITY,baseVolume);

}

/***************************************************************************************************/
Beeper::Beeper(uint32_t pin, uint8_t baseVolume) {
/***************************************************************************************************/
/*#ifdef SOUND_DEBUG
    SerialPort.print("Beeper - pin : ");
		SerialPort.println(pin);
    SerialPort.print("Beeper - volume : ");
		SerialPort.println(baseVolume);
#endif //BUTTON_DEBUG

  // save volume 
 // volume = baseVolume;
  
  // set threshold 
  setThresholds(BEEP_VELOCITY_DEFAULT_SINKING_THRESHOLD, BEEP_VELOCITY_DEFAULT_CLIMBING_THRESHOLD, BEEP_VELOCITY_DEFAULT_NEAR_CLIMBING_SENSITIVITY);
  
  // init private vars 
  beepStartTime = 0;
  beepState = 0;
  beepType = BEEP_TYPE_SILENT;	
	
//	toneHAL.init(pin);*/
  init(BEEP_VELOCITY_DEFAULT_SINKING_THRESHOLD, BEEP_VELOCITY_DEFAULT_CLIMBING_THRESHOLD, BEEP_VELOCITY_DEFAULT_NEAR_CLIMBING_SENSITIVITY,baseVolume);
}

/***************************************************************************************************/
void Beeper::init(double sinkingThreshold, double climbingThreshold, double nearClimbingSensitivity, uint8_t baseVolume) {
/***************************************************************************************************/
#ifdef SOUND_DEBUG
    SerialPort.println("Beeper : init");
    SerialPort.print("Beeper - volume : ");
		SerialPort.println(baseVolume);
#endif //BUTTON_DEBUG

  /* save volume */
  //volume = baseVolume;
  
  /* set threshold */
  setThresholds(sinkingThreshold, climbingThreshold, nearClimbingSensitivity);
  
  /* init private vars */
  beepStartTime = 0;
  beepState = 0;
  beepType = BEEP_TYPE_SILENT;

	//init le duty / cycle
	CLIMBING_BEEP_HIGH_LENGTH = 500;
	CLIMBING_BEEP_LOW_LENGTH  = 500;
	CLIMBING_BEEP_LENGTH = (CLIMBING_BEEP_HIGH_LENGTH + CLIMBING_BEEP_LOW_LENGTH);

}

/***************************************************************************************************/
void Beeper::setThresholds(double sinkingThreshold, double climbingThreshold, double nearClimbingSensitivity) {
/***************************************************************************************************/

  beepSinkingThreshold  = sinkingThreshold;
  beepGlidingThreshold  = climbingThreshold - nearClimbingSensitivity;
  beepClimbingThreshold = climbingThreshold;
}

/***************************************************************************************************/
void Beeper::setVolume(uint8_t newVolume) {
/***************************************************************************************************/
#ifdef SOUND_DEBUG
    SerialPort.print("Beeper - setvolume : ");
		SerialPort.println(newVolume);
#endif //BUTTON_DEBUG

//  volume = newVolume;
  toneHAL.setVolume(newVolume);
}

/***************************************************************************************************/
uint8_t  Beeper::getVolume() {
/***************************************************************************************************/
/*  volume = toneHAL.getVolume();
  return volume;*/
	
  uint8_t _volume = toneHAL.getVolume();
  return _volume;	
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

#ifdef BEEPERVARIABLE	
	double tmpcycle;
	double tmpduty;
#endif

  /* compute the beep freq that depend to beep type */
  switch( beepType ) {
  case BEEP_TYPE_SINKING :
    beepFreq = varioXBeeper.getFrequence(velocity); //SINKING_BEEP_FREQ_COEFF * velocity + SINKING_BEEP_BASE_FREQ;
	
#ifdef BEEPERVARIABLE	
		tmpcycle = varioXBeeper.getCycle(velocity);
		tmpduty  = varioXBeeper.getDuty(velocity) / 100.0;
		CLIMBING_BEEP_HIGH_LENGTH = tmpcycle * tmpduty;
		CLIMBING_BEEP_LOW_LENGTH  = tmpcycle * (1 - tmpduty);
		CLIMBING_BEEP_LENGTH = tmpcycle;  //(CLIMBING_BEEP_HIGH_LENGTH + CLIMBING_BEEP_LOW_LENGTH);
#endif //BEEPERVARIABLE
    break;

  case BEEP_TYPE_SILENT :
    beepFreq = 0.0;
    break;

  case BEEP_TYPE_GLIDING :
    beepFreq = varioXBeeper.getFrequence(velocity); //CLIMBING_BEEP_FREQ_COEFF * velocity + CLIMBING_BEEP_BASE_FREQ;

#ifdef BEEPERVARIABLE	
		tmpcycle = varioXBeeper.getCycle(velocity);
		tmpduty  = varioXBeeper.getDuty(velocity) / 100.0;
		CLIMBING_BEEP_HIGH_LENGTH = tmpcycle * tmpduty;
		CLIMBING_BEEP_LOW_LENGTH  = tmpcycle * (1 - tmpduty);
		CLIMBING_BEEP_LENGTH = tmpcycle; //(CLIMBING_BEEP_HIGH_LENGTH + CLIMBING_BEEP_LOW_LENGTH);
/*		CLIMBING_BEEP_LENGTH = varioXBeeper.getCycle(velocity);
		tmpduty  = varioXBeeper.getDuty(velocity) / 100.0;
		CLIMBING_BEEP_HIGH_LENGTH = (CLIMBING_BEEP_LENGTH * tmpduty);
		CLIMBING_BEEP_LOW_LENGTH  = (CLIMBING_BEEP_LENGTH * (1 - tmpduty));*/
#endif //BEEPERVARIABLE
    break;

  case BEEP_TYPE_CLIMBING :
    beepFreq = varioXBeeper.getFrequence(velocity); //CLIMBING_BEEP_FREQ_COEFF * velocity + CLIMBING_BEEP_BASE_FREQ;

#ifdef BEEPERVARIABLE	
		tmpcycle = varioXBeeper.getCycle(velocity);
		tmpduty  = varioXBeeper.getDuty(velocity) / 100.0;
		CLIMBING_BEEP_HIGH_LENGTH = tmpcycle * tmpduty;
		CLIMBING_BEEP_LOW_LENGTH  = tmpcycle * (1 - tmpduty);
		CLIMBING_BEEP_LENGTH = tmpcycle; //(CLIMBING_BEEP_HIGH_LENGTH + CLIMBING_BEEP_LOW_LENGTH);
#endif //BEEPERVARIABLE
    break;
  }

#ifdef SOUND_DEBUG
		SerialPort.print("CYCLE : ");
		SerialPort.println(tmpcycle);
		SerialPort.print("DUTY : ");
		SerialPort.println(tmpduty);
    SerialPort.print("CLIMBING_BEEP_HIGH_LENGTH : ");
    SerialPort.println(CLIMBING_BEEP_HIGH_LENGTH);
    SerialPort.print("CLIMBING_BEEP_LOW_LENGTH : ");
    SerialPort.println(CLIMBING_BEEP_LOW_LENGTH);
    SerialPort.print("CLIMBING_BEEP_LENGTH : ");
    SerialPort.println(CLIMBING_BEEP_LENGTH);
#endif //BUTTON_DEBUG

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

//
//un beep patern est un truc du genre
// ----____
// c'est a dire une bip suivi d'un silence
// a la base il est indépendant de la vitesse à laquelle on le lis
// pour les bip rapides il faut le lire plus vite et pour les bips
// lents moins vite.
// Par contre il peut être avec un long bip et un court silence :
// --------__
// ou à l'inverse un court beep et un long silence
// --________
// Mais pour l'instant dans le code actuel le partern reste fixe. Et
// donc le duty cycle ne change pas. 
//
// l'objectif de cette fonction est de savoir où on en est de la lecture du patern
// elle doit mettre à jour beepPaternPosition
// comme le partern boucle la position de départ n'est pas toujours au début du patern
// elle est donnée par beepPaternBasePosition et elle a démarré au temps beepStartTime

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
  // si le beep n'est pas une alarm ou si c'est le bip de degeulante il
  // n'y a pas de patern. Cette fonction ne sert à rien.
  // on retourne
  if( !haveAlarm &&
      (beepType == BEEP_TYPE_SINKING || beepType == BEEP_TYPE_SILENT) ) {
    return;
  }
  
  //on calcule le temps qui s'est écoulé depuis le début de lecture du patern
  //en seconde.
  unsigned long currentTime = millis();
  double currentLength = (double)(currentTime - beepStartTime);

  /*******************************************/
  /* does the position depends on velocity ? */
  /*******************************************/
  // il y a des partern qui doivent être joué à des vitesses différentes (bip de montée)
  // et d'autre a vitesse constante (les alarmes)
  if( !haveAlarm &&
      beepType == BEEP_TYPE_CLIMBING ) {
		//BEEP DE MONTE
    // ici on calcule la position dans le patern en fonction de la vitesse
    // plus la vitesse est élevée plus on avance vite dans le patern
//    currentLength *= (beepVelocity * CLIMBING_BEEP_VELOCITY_FILTER_COEFF + CLIMBING_BEEP_VELOCITY_FILTER_BASE);

    /* avoid going backward */
    //ici c'est subtil
    // on ne veut pas revenir en arrière de la lecture. Ca pourrait arriver si le vitesse baisse
    // trop vite et qu'il n'y a pas assez de temps qui s'est écoulé entre temps.
    //
    // Du coup on vérifie que la nouvelle position (currentLength + beepPaternBasePosition)
    // est bien supérieur à la précédente (le précédent beepPaternPosition).
    // et on ne met a jour beepPaternPosition que si on avance.
    if( currentLength + beepPaternBasePosition > beepPaternPosition ) {
      beepPaternPosition = currentLength + beepPaternBasePosition;
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
  // ici on calcule la nouvelle position quand le patern boucle et que l'on a dépassé
  // la taille du patern
  else {
    double loopingPaternLength;
    if(  beepType == BEEP_TYPE_GLIDING ) {
      loopingPaternLength = GLIDING_BEEP_LENGTH;
    }else {
      loopingPaternLength = CLIMBING_BEEP_LENGTH;
    }

    // tant que la position sort du patern
    // on soustrait la longeur du patern
    // afin de boucler
    // en général il n'y aura qu'une soustraction
    while( beepPaternPosition > loopingPaternLength ) {
        beepPaternPosition -= loopingPaternLength;
        beepStartTime = millis();
        beepPaternBasePosition = beepPaternPosition;
				//On met à jour CLIMBING_BEEP_HIGH_LENGTH
    }
  }
}


/***************************************************************************************************/
void Beeper::setTone() {
/***************************************************************************************************/
  
#ifdef SOUND_DEBUG
//			SerialPort.print("Beeper - setTone Volume : ");
//			SerialPort.println(volume);
#endif //SOUND_DEBUGG
	
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
					toneHAL.tone(CLIMBING_ALARM_FREQ); //, volume);					
					bst_set(BEEP_HIGH);
				} else if( bst_isset(BEEP_NEW_FREQ) ) {
					toneHAL.tone(CLIMBING_ALARM_FREQ); //, volume);
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
				toneHAL.tone(SINKING_ALARM_FREQ); //, volume);
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
				toneHAL.tone(beepFreq);	//, volume);
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
						toneHAL.tone(beepFreq);	//, volume);
						bst_set(BEEP_HIGH);
					} else if( bst_isset(BEEP_NEW_FREQ) ) {
						toneHAL.tone(beepFreq);	//, volume);
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
 					
					toneHAL.tone(beepFreq);	//, volume);
					bst_set(BEEP_HIGH);
				} else if( bst_isset(BEEP_NEW_FREQ) ) {
#ifdef SOUND_DEBUG
					SerialPort.println("Beeper - setTone : BEEP_TYPE_CLIMBING / BEEP_NEW_FREQ");
#endif //SOUND_DEBUGG
 					toneHAL.tone(beepFreq);	//, volume);
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
    toneHAL.tone(fHz);	//, volume);   //volume);
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
void Beeper::setFrequency(uint32_t fHz, uint8_t volume) {
/***************************************************************************************************/
	if (fHz ) {
    toneHAL.tone(fHz, volume);   //volume);
	}
	else {
		toneHAL.noTone();
	}
}


/***************************************************************************************************/
void Beeper::generateTone(uint32_t fHz, int ms, uint8_t volume) {
/***************************************************************************************************/
    setFrequency(fHz, volume);
    delay(ms);
    setFrequency(0);
}

/***************************************************************************************************/
void Beeper::tone(uint32_t fHz, uint8_t volume) {
/***************************************************************************************************/
	setFrequency(fHz, volume);
}

/***************************************************************************************************/
void Beeper::noTone(void) {
/***************************************************************************************************/
	toneHAL.noTone();
}
