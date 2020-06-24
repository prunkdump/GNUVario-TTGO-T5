/* utility -- 
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
/*                           Utility                                             */
/*                                                                               */
/*  version    Date        Description                                           */
/*    1.0      05/07/19                                                          */
/*    1.1      24/09/19    Ajout deep_sleep                                      */
/*    1.2      15/10/19    Ajout test SDCARD                                     */
/*    1.3      23/11/19    Modification deep sleep                               */
/*    1.4      28/11/19    Modif changement librairie sdfat                      */
/*    1.5      20/01/20    Modif deep_sleep                                      */
/*                                                                               */
/*********************************************************************************/


#ifndef UTILITY_H
#define UTILITY_H

#include <Arduino.h>
#include <VarioSettings.h>

#include <HardwareConfig.h>

//#include <EnergySaving.h>


/******************************************************/
/******************************************************/

void indicateUncalibratedAccelerometer();
void indicatePowerDown();
void indicateFaultMS5611();
void indicateFaultMPU9250();
void indicateFaultSDCARD();
void signalBeep(double freq, unsigned long duration, int count);
int8_t percentBat(double targetVoltage);
void deep_sleep(String Message);
#ifdef SDFAT_LIB
void printSdDirectory(SdFile dir, int numTabs);
#else
void printSdDirectory(File dir, int numTabs);
#endif
bool TestSDCARD(bool init);

extern VarioSettings GnuSettings;
#ifdef SDFAT_LIB
extern SdFile fileIgc;
#else
extern File fileIgc;
#endif

//extern Beeper beeper;

#endif
