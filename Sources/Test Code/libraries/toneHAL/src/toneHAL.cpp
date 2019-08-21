/* toneHAL -- tone HAL
 *
 * Copyright 2019 Jean-philippe GOI
 * 
 * This file is part of toneHAL.
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
/*                           Libraries ToneHal                                   */
/*                                                                               */
/*  version    Date     Description                                              */
/*    1.0    20/01/19                                                            */
/*    1.1    24/01/19   Réecriture des classes                                   */
/*                      répartition en plusieurs fichiers                        */
/*    1.2    26/01/19   Modifications mineures                                   */
/*    1.3    09/02/19   Ajout TONEHAL_EXTENDED_VOLUME							               */
/*    1.4    02/03/19   Ajout ESP32                                              */
/*    1.4.1  12/03/19   Modifications mineures								                   */
/*    1.4.2  16/03/19   Ajout description et licence en début de fichier         */
/*    1.4.3  08/04/19   ToneDAC pour l'ESP32                                     */
/*    1.4.4  25/04/2019 Modification DebugConfig.h et HardwareConfig.h           */
/*    1.4.5  05/06/2019 Déclaration de l'instance dans toneHAL.cpp						   */
/*    1.4.6    10/06/2019 Ajout gestion ampli class D                            */ 
/*                                                                               */
/*********************************************************************************/

#include "toneHAL.h"
#include <Arduino.h>

#if defined (TONEHAL_EXTENDED_VOLUME)

/***********************************/
void ToneHal::setVolume(uint8_t newVolume) {
/***********************************/

  _volume = newVolume;
}

/***********************************/
uint8_t ToneHal::getVolume() {
/***********************************/

  return _volume;
}

/***********************************/
void ToneHal::mute(bool newMuteState) {
/***********************************/
  /* stop tone if needed */
  if( newMuteState ) {
    noTone();
  }

  /* save */
  _toneMuted = newMuteState;
}

#endif //TONEHAL_EXTENDED_VOLUME

ToneHAL toneHAL;
