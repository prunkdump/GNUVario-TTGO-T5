/* GPSSentences -- Generate some standard GPS sentences 
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

#ifndef LXNAV_SENTENCE_H
#define LXNAV_SENTENCE_H

#include <Arduino.h>
#include <digit.h>

// $LXWP0,logger_stored, airspeed, airaltitude,vario,,,,,, hdg, windspeed*CS
// 0 loger_stored : [Y|N]
// 1 TAS [km/h]
// 2 baroaltitude [m]
// 3 vario[m/s]
// 4-8 not used
// 9 heading of glider
// 10 windcourse [deg]
// 11 windspeed [km/h] 
/* no special character for the first digit (alti), just for vario "V" and parity "P" */
#define LXNAV_SENTENCE_TAG "$LXWP0,Y,,,V,V,V,V,V,V,,,*P\r\n"
#define LXNAV_SENTENCE_TAG_SIZE 29
#define LXNAV_SENTENCE_ALTI_POS 10
#define LXNAV_SENTENCE_VARIO_POS1 11
#define LXNAV_SENTENCE_VARIO_POS2 13
#define LXNAV_SENTENCE_VARIO_POS3 15
#define LXNAV_SENTENCE_VARIO_POS4 17
#define LXNAV_SENTENCE_VARIO_POS5 19
#define LXNAV_SENTENCE_VARIO_POS6 21
#define LXNAV_SENTENCE_PARITY_POS 26
#define LXNAV_SENTENCE_ALTI_PRECISION 1
#define LXNAV_SENTENCE_VARIO_PRECISION 2

class LxnavSentence
{

public:
  LxnavSentence() : valueDigit() {}
  void begin(double alti, double vario);
  bool available(void);
  uint8_t get(void);

private:
  double vario; //alti is directly initialized
  Digit valueDigit;
  uint8_t parity;
  HexDigit parityDigit;
  uint8_t tagPos;
};

#endif
