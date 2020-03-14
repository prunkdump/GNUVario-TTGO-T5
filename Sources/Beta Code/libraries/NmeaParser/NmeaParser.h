/* NmeaParser -- Parse NMEA GPS sentences
 *
 * Copyright 2016-2019 Baptiste PELLEGRIN, Jean-Philippe
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

/* 
 *********************************************************************************
 *********************************************************************************
 *                                                                               *
 *                           NmeaParser                                          *
 *                                                                               *
 *  version    Date     Description                                              *
 *    1.0                                                                        *
 *    1.0.1  04/01/20   Ajout décodage Compas                                    *
 *    1.0.2  27/01/20   Ajout décodage Long / Lat                                *
 *    1.0.3  09/02/20   Ajout getLongDegree, getLatDegree et DegreesToDegMinSec  *
 *    1.0.4  10/02/20   Correction getlongDegree et DegreesToDegMinSec           *
 *    1.0.6  25/02/20   Correction calcul long /lat                              *
 *    1.0.7  28/02/20   Correction DegreesToDegMinSec                            *
 *    1.0.8  01/03/20   Modification affichage lat/long                          *
 *                                                                               *
 *********************************************************************************/

#ifndef NMEA_PARSER_H
#define NMEA_PARSER_H

#include <Arduino.h>
#include <VarioSettings.h>


/*********************/
/* the GPS sentences */
/*********************/

/* precision = 10^(number of digit after dot in gps ouput) */
#ifndef NMEA_RMC_SPEED_PRECISION
#define NMEA_RMC_SPEED_PRECISION 100.0
#endif

/*#ifndef NMEA_RMC_LAT_PRECISION
#define NMEA_RMC_LAT_PRECISION 1000000.0
#endif*/

/*#ifndef NMEA_RMC_LONG_PRECISION
#define NMEA_RMC_LONG_PRECISION 1000000.0
#endif*/

#ifndef NMEA_RMC_TRACK_PRECISION
#define NMEA_RMC_TRACK_PRECISION 100.0
#endif

#ifndef NMEA_GGA_TIME_PRECISION
#define NMEA_GGA_TIME_PRECISION 1000
#endif

#ifndef NMEA_GGA_ALTI_PRECISION
#define NMEA_GGA_ALTI_PRECISION 10.0
#endif

/* position in NMEA sentence */
#define NMEA_PARSER_RMC_LAT_POS   							3
#define NMEA_PARSER_RMC_LAT_DIR_POS							4
#define NMEA_PARSER_RMC_LONG_POS								5
#define NMEA_PARSER_RMC_LONG_DIR_POS						6
#define NMEA_PARSER_RMC_SPEED_POS 							7
#define NMEA_PARSER_RMC_TRACK_POS 							8
#define NMEA_PARSER_RMC_DATE_POS 								9
#define NMEA_PARSER_RMC_MAGNETIC_PAS 						10
#define NMEA_PARSER_RMC_MAG_SENS_POS 						11
#define NMEA_PARSER_GGA_TIME_POS 								1
#define NMEA_PARSER_GGA_SATELLITE_COUNT_POS 		7
#define NMEA_PARSER_GGA_PRECISION_POS 					8
#define NMEA_PARSER_GGA_ALTITUDE_POS 						9


class NmeaParser {

 public :
 NmeaParser() : satelliteCount(0), state(0) { }
  uint8_t satelliteCount;
  uint32_t time;
  uint32_t date;
  uint16_t precision;
  uint16_t altitude;
  uint16_t speed;
	double   track;
	double	 magnetic;
	bool     magneticSens;
	uint32_t latitude;
	char		 latDir;
	char     longDir;
	uint32_t longitude;

  void beginRMC(void);
  void beginGGA(void);
  void feed(uint8_t c);
  bool haveNewAltiValue(void);
  bool haveNewSpeedValue(void);
  bool haveDate(void);
	bool haveBearing(void);
	bool haveLongitude(void);
  bool haveLatitude(void);
  double getAlti(void);
  double getSpeed(void); //in KMH
	double getSpeed_no_unset(void);
	double getBearing(void);
	double getLong(void);
	char   getLongDir();
	String getLongDegree(void);
	String getLongitude(boolean majState = true);
	double getLat(void);
	char   getLatDir(void);
	String getLatDegree(void);
	String getLatitude(boolean majState = true);
  bool isParsing(void);
  bool isParsingRMC(void);
  bool isParsingGGA(void);
	String Bearing_to_Ordinal(float bearing);

 private :
  uint16_t state;
  uint8_t commaCount;
  uint32_t value;
	char valuechar;
	String DegreesToDegMinSec(double x);
	int comptdec = 0;
	String tmpstr = "";
	
	double NMEA_RMC_LAT_PRECISION  = 1000000.0;
	double NMEA_RMC_LONG_PRECISION = 1000000.0;

};

#endif


//$GNGGA,064607.000,4546.2282,N,00311.6590,E,1,05,2.6,412.0,M,0.0,M,,*77
//$GNGGA,055828.000,4546.2305,N,00311.6597,E,1,09,1.8,385.2,M,0.0,M,,*75

//$GNRMC,064607.000,A,4546.2282,N,00311.6590,E,0.76,0.00,230619,,,A*7D
//$GNRMC,055828.000,A,4546.2305,N,00311.6597,E,0.25,0.00,070120,,,A*78

//$LXWP0,Y,,189.0,-0.11,,,,,,,,*67

/*
 $GPRMC,225446,A,4916.45,N,12311.12,W,000.5,054.7,191194,020.3,E*68
225446 = Heure du Fix 22:54:46 UTC
A = Alerte du logiciel de navigation ( A = OK, V = warning (alerte)
4916.45,N = Latitude 49 deg. 16.45 min North
12311.12,W = Longitude 123 deg. 11.12 min West
000.5 = vitesse sol, Noeuds
054.7 = cap (vrai)
191194 = Date du fix 19 Novembre 1994
020.3,E = Déclinaison Magnétique 20.3 deg Est
*68 = checksum obligatoire


$GPRMC,123519,A,4807.038,N,01131.000,E,022.4,084.4,230394,003.1,W*6A

Where:
     RMC          Recommended Minimum sentence C
     123519       Fix taken at 12:35:19 UTC
     A            Status A=active or V=Void.
     4807.038,N   Latitude 48 deg 07.038' N
     01131.000,E  Longitude 11 deg 31.000' E
     022.4        Speed over the ground in knots
     084.4        Track angle in degrees True
     230394       Date - 23rd of March 1994
     003.1,W      Magnetic Variation
     *6A          The checksum data, always begins with *

 $GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47

Where:
     GGA          Global Positioning System Fix Data
     123519       Fix taken at 12:35:19 UTC
     4807.038,N   Latitude 48 deg 07.038' N
     01131.000,E  Longitude 11 deg 31.000' E
     1            Fix quality: 0 = invalid
                               1 = GPS fix (SPS)
                               2 = DGPS fix
                               3 = PPS fix
             4 = Real Time Kinematic
             5 = Float RTK
                               6 = estimated (dead reckoning) (2.3 feature)
             7 = Manual input mode
             8 = Simulation mode
     08           Number of satellites being tracked
     0.9          Horizontal dilution of position
     545.4,M      Altitude, Meters, above mean sea level
     46.9,M       Height of geoid (mean sea level) above WGS84
                      ellipsoid
     (empty field) time in seconds since last DGPS update
     (empty field) DGPS station ID number
     *47          the checksum data, always begins with *

*/
