/* VarioSound -- Gestion des parametres du son
 *
 * Copyright 2020 Jean-philippe GOI
 * 
 * .
 *
 * ToneHAL is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ToneHAL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

/*********************************************************************************/
/*                                                                               */
/*                           VarioSettings                                       */
/*                                                                               */
/*  version    Date     Description                                              */
/*    1.0    27/06/20                                                            */
/*    1.0.1  27/09/20   Modif valeur de retour getFrenquence                     */
/*                                                                               */
/*********************************************************************************/

#ifndef _VARIO_XBEEPER_H_
#define _VARIO_XBEEPER_H_

#include <Arduino.h>

#include "DebugConfig.h"
#include <HardwareConfig.h>

#ifdef HAVE_SDCARD
#include <sdcardHAL.h>
#endif

typedef struct
 {
     float vario;		     //   m/s
     int frequence;			 //		hertz
     int cycle;					 //		ms
		 int duty;					 //   %
 }  record_tone_type;


class VarioXBeeper {

public:
  boolean init(void);
  boolean readSDSettings(char *FileName);
	void majVarioSettings(boolean *ModifiedValue);
	uint16_t getFrequence(float vario);
	int getCycle(float vario);
	int getDuty(float vario);
	
protected:

#ifdef HAVE_SDCARD
#ifdef SDFAT_LIB
	SdFile myFile;
#else //SDFAT_LIB
	File myFile;
#endif //SDFAT_LIB
#endif

//DATA
	float ClimbToneOnThreshold;
	boolean ClimbToneOnThresholdMaj  = false;
	float ClimbToneOffThreshold;
	boolean ClimbToneOffThresholdMaj = false;
	float SinkToneOnThreshold;
	boolean SinkToneOnThresholdMaj   = false;
	float SinkToneOffThreshold;
	boolean SinkToneOffThresholdMaj  = false; 
	record_tone_type tone[20];
	int maxRecord = 0;
	int indexTone = 0;
	int indexToneZero;

	boolean readLines(char* line);
	void get_value(char * chaine, byte index, char * delimiter, char * get_val);
	boolean decodeLines(char* line);
};

extern VarioXBeeper varioXBeeper;

#endif
