/* varioStat -- 
 *
 * Copyright 2019 Jean-philippe GOI
 * 
 * This file is part of GnuVario-E.
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
/*                           VarioStat                                           */
/*                                                                               */
/*  version    Date        Description                                           */
/*    1.0      06/07/19                                                          */
/*    1.1      21/07/19    Correction getdate/setdate                            */
/*    1.1.1    22/07/19    Modification du taux d'enregistrement des statistiques*/
/*    1.1.2    08/08/19    Correction readEeprom et writeEeprom                  */
/*                                                                               */
/*********************************************************************************/


#ifndef VARIOSTAT_H
#define VARIOSTAT_H

#include <Arduino.h>
#include <HardwareConfig.h>

#ifndef FLY_STAT_HEADER_EEPROM_ADDRESS
#define FLY_STAT_HEADER_EEPROM_ADDRESS 0x230 
#endif

#ifndef FLY_STAT_EEPROM_TAG
#define FLY_STAT_EEPROM_TAG 2025
#endif

struct S_Stat {
   double        MaxAlti;
   double        MaxSpeed;
   double        MaxVario;
   double        MinVario;
   bool          EnableRecord;
   int8_t 		   time[3];
	 int8_t        duree[3];
	 uint16_t       date[4];
};

/********************/
/* The stat class   */
/********************/

class VarioStat {
 public:
   void Begin(void);
   void Enable(void);
   void Disable(void);
   void SetAlti(double alti);
   void SetSpeed(double speed);
   void SetVario(double vario);
   void SetDuration(int8_t* dureeValue);
	 void SetTime(int8_t* timeValue);
	 void SetDate(uint8_t* dateValue);
   double GetAlti(void);
   double GetVarioMin(void);
   double GetVarioMax(void);
   double GetSpeed(void);
   void GetDuration(int8_t* dureeValue);
	 void GetTime(int8_t* timeValue);
	 void GetDate(uint8_t* dateValue);
   void Display(void);
   bool Handle(void);
	 void ForceWrite(void);

 private:
   unsigned long Timer;
   double        MaxAlti;
   double        MaxSpeed;
   double        MaxVario;
   double        MinVario;
   bool          EnableRecord;
   int8_t 		   time[3];
	 int8_t        duree[3];
	 uint8_t       date[3];
	 int					 numberStat;	
	 bool          firsttime;
	 struct S_Stat dataStat;
     
   void ReadEeprom(void);
   void WriteEeprom(void);
};

/*int EEPROMAnythingWrite(int pos, char *character, int length);
int EEPROMAnythingRead(int pos, char *character, int length);*/

#endif
