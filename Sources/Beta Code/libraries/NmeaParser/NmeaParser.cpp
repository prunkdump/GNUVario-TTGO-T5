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
 *    1.0.5  12/02/20   Ajout tracedebug                                         *
 *    1.0.6  25/02/20   Correction calcul long /lat                              *
 *    1.0.7  28/02/20   Correction DegreesToDegMinSec                            *
 *    1.0.8  01/03/20   Modification affichage lat/long                          *
 *                                                                               *
 *********************************************************************************/

#include <NmeaParser.h>

#include <Arduino.h>
#include <DebugConfig.h>

#ifdef NMEAPARSER_DEBUG
#define ARDUINOTRACE_ENABLE 1
#else
#define ARDUINOTRACE_ENABLE 0
#endif

#define ARDUINOTRACE_SERIAL SerialPort
#include <ArduinoTrace.h>

#define parserState_set(bit) state |= (1 << bit)
#define parserState_unset(bit) state &= ~(1 << bit)
#define parserState_isset(bit) (state & (1 << bit))

#define PARSE_RMC 0
#define PARSE_GGA 1
#define DIGIT_PARSED 2
#define HAVE_NEW_SPEED_VALUE 6
#define HAVE_NEW_ALTI_VALUE 4
#define HAVE_DATE 5
#define HAVE_BEARING 7
#define HAVE_LONG 8
#define HAVE_LAT  9

#define KNOTS_TO_KMH 1.852 

void NmeaParser::beginRMC(void) {

  commaCount = 0;
  value = 0;
  parserState_set(PARSE_RMC);
  parserState_unset(DIGIT_PARSED);
}

void NmeaParser::beginGGA(void) {

  commaCount = 0;
  value = 0;
  parserState_set(PARSE_GGA);
  parserState_unset(DIGIT_PARSED);
}

 
void NmeaParser::feed(uint8_t c) {

  /* maybe we need to stop parsing */
  if( c == '*' ) {
    parserState_unset(PARSE_RMC);
    parserState_unset(PARSE_GGA);
  }

  if( ! isParsing() ) {
    return;
  }

  /* digit case */
  if( c != ',' ) {
    if( c >= '0' && c <= '9' ) {  
      value *= 10;
      value += c - '0';
      parserState_set(DIGIT_PARSED);
			comptdec++;
      char tmpchar = c;
			tmpstr = tmpstr + tmpchar;
    } else if ( c != '.') {
			valuechar = c;
		} else {
			comptdec = 0;
	  }
  }

  /* comma case */
  else {
    commaCount++;
   
    /* if we have a value to parse */
    if( parserState_isset(DIGIT_PARSED) ) {

      /* RMC case */
      if( parserState_isset(PARSE_RMC) ) {

	/* RMC speed */
				if( commaCount == NMEA_PARSER_RMC_SPEED_POS ) { 
	
#ifdef NMEAPARSER_DEBUG
					SerialPort.print("Speed : ");
					SerialPort.println((((double)value)/NMEA_RMC_SPEED_PRECISION)*KNOTS_TO_KMH);
#endif //NMEAPARSER_DEBUG

					speed = value;
					parserState_set(HAVE_NEW_SPEED_VALUE);
		
#ifdef NMEAPARSER_DEBUG
					SerialPort.print("state set : ");
					SerialPort.println(state);
#endif //NMEAPARSER_DEBUG
		
				}

	/* RMC date */
				else if( commaCount == NMEA_PARSER_RMC_DATE_POS ) {
					date = value;
					parserState_set(HAVE_DATE);
				}

				else if( commaCount == NMEA_PARSER_RMC_TRACK_POS ) {
					track = value;
					parserState_set(HAVE_BEARING);
				}
				
				else if( commaCount == NMEA_PARSER_RMC_LONG_POS ) {
					longitude = value;
//          longitude = tmpstr.toInt();
					NMEA_RMC_LONG_PRECISION = 1;
					for (int i=0; i < comptdec; i++) NMEA_RMC_LONG_PRECISION *= 10;				
					NMEA_RMC_LONG_PRECISION *= 100;

#ifdef NMEAPARSER_DEBUG
          SerialPort.println("");
					SerialPort.print("comptdec : ");
					SerialPort.println(comptdec);					
					SerialPort.print("longitude : ");
					SerialPort.print(longitude);
					SerialPort.print(" / ");
					double tmpdouble = longitude /NMEA_RMC_LONG_PRECISION;
					SerialPort.println(tmpdouble);
					DUMP(NMEA_RMC_LONG_PRECISION);
					SerialPort.println(tmpstr);
					SerialPort.println(value);
#endif //NMEAPARSER_DEBUG
				}

				else if( commaCount == NMEA_PARSER_RMC_LAT_POS ) {
					latitude = value;
					NMEA_RMC_LAT_PRECISION = 1;
					for (int i=0; i < comptdec; i++) NMEA_RMC_LAT_PRECISION *= 10;				
					NMEA_RMC_LAT_PRECISION *= 100;
					
#ifdef NMEAPARSER_DEBUG
          SerialPort.println("");
					SerialPort.print("comptdec : ");
					SerialPort.println(comptdec);					
					SerialPort.print("latitude : ");
					SerialPort.print(latitude);
					SerialPort.print(" / ");
					double tmpdouble = latitude /NMEA_RMC_LAT_PRECISION;					
					SerialPort.println(tmpdouble);
					DUMP(NMEA_RMC_LONG_PRECISION);
					SerialPort.println(tmpstr);
					SerialPort.println(value);
#endif //NMEAPARSER_DEBUG
				}
     }

      /* GGA case */
      else {

				/* GGA time */
				if( commaCount == NMEA_PARSER_GGA_TIME_POS ) {
					time = value/NMEA_GGA_TIME_PRECISION;
				}

				/* GGA satellite count */
				else if( commaCount == NMEA_PARSER_GGA_SATELLITE_COUNT_POS ) {
					satelliteCount = value;
				}

				/* GGA precision */
				else if( commaCount == NMEA_PARSER_GGA_PRECISION_POS ) {
					precision = value;
				}

				/* GGA altitude */
				else if( commaCount == NMEA_PARSER_GGA_ALTITUDE_POS ) {
					altitude = value;
					parserState_set(HAVE_NEW_ALTI_VALUE);
				}
      }
    } else {
      if( parserState_isset(PARSE_RMC) ) {
				if( commaCount == NMEA_PARSER_RMC_LONG_DIR_POS ) {
					longDir = valuechar;
					parserState_set(HAVE_LONG);
				}
				else 	if( commaCount == NMEA_PARSER_RMC_LAT_DIR_POS ) {
					latDir = valuechar;
					parserState_set(HAVE_LAT);
				}
			}
		}

    /* reset value */
    value = 0;
		comptdec = 0;
		tmpstr = "";
    parserState_unset(DIGIT_PARSED);
  }
}	

  
bool NmeaParser::haveNewAltiValue(void) {

  return parserState_isset(HAVE_NEW_ALTI_VALUE);
}

bool NmeaParser::haveNewSpeedValue(void) {

#ifdef NMEAPARSER_DEBUG
    SerialPort.print("state get : ");
    SerialPort.println(state);
    SerialPort.print("parserState  : ");
    SerialPort.println(parserState_isset(HAVE_NEW_SPEED_VALUE));		
#endif //NMEAPARSER_DEBUG

  return parserState_isset(HAVE_NEW_SPEED_VALUE);
}

bool NmeaParser::haveDate(void) {

  return parserState_isset(HAVE_DATE);
}

double NmeaParser::getAlti(void) {

  parserState_unset(HAVE_NEW_ALTI_VALUE);
  return ((double)altitude)/NMEA_GGA_ALTI_PRECISION;
}
  
double NmeaParser::getSpeed(void) {

#ifdef NMEAPARSER_DEBUG
    SerialPort.println("GetSpeed");
#endif //NMEAPARSER_DEBUG

  parserState_unset(HAVE_NEW_SPEED_VALUE);
  return ((((double)speed)/NMEA_RMC_SPEED_PRECISION)*KNOTS_TO_KMH);
}

bool NmeaParser::haveBearing(void) {

  return parserState_isset(HAVE_BEARING);
}

double NmeaParser::getBearing(void) {

  parserState_unset(HAVE_BEARING);
  return ((double)track)/NMEA_RMC_TRACK_PRECISION;
}

bool NmeaParser::haveLongitude(void) {

  return parserState_isset(HAVE_LONG);
}

bool NmeaParser::haveLatitude(void) {

  return parserState_isset(HAVE_LAT);
}

double NmeaParser::getSpeed_no_unset(void) {

#ifdef NMEAPARSER_DEBUG
    SerialPort.println("GetSpeed no unset");
#endif //NMEAPARSER_DEBUG

  return ((((double)speed)/NMEA_RMC_SPEED_PRECISION)*KNOTS_TO_KMH);
}

bool NmeaParser::isParsing(void) {

  return ( parserState_isset(PARSE_RMC) || parserState_isset(PARSE_GGA) );
}

bool NmeaParser::isParsingRMC(void) {

  return parserState_isset(PARSE_RMC);
}
bool NmeaParser::isParsingGGA(void) {

  return parserState_isset(PARSE_GGA);
}

String NmeaParser::Bearing_to_Ordinal(float bearing) {
  if (bearing >=    0.0  && bearing < 11.25  ) return "N";
  if (bearing >=  11.25  && bearing < 33.75 )  return "NNE";
  if (bearing >=  33.75  && bearing < 56.25 )  return "NE";
  if (bearing >=  56.25  && bearing < 78.75 )  return "ENE";
  if (bearing >=  78.75  && bearing < 101.25 ) return "E";
  if (bearing >=  101.25 && bearing < 123.75 ) return "ESE";
  if (bearing >=  123.75 && bearing < 146.25 ) return "SE";
  if (bearing >=  146.25 && bearing < 168.75 ) return "SSE";
  if (bearing >=  168.75 && bearing < 191.25 ) return "S";
  if (bearing >=  191.25 && bearing < 213.75 ) return "SSW";  
  if (bearing >=  213.75 && bearing < 236.25 ) return "SW";
  if (bearing >=  236.25 && bearing < 258.25 ) return "WSW";
  if (bearing >=  258.25 && bearing < 281.25 ) return "W";
  if (bearing >=  281.25 && bearing < 303.75 ) return "WNW";
  if (bearing >=  303.75 && bearing < 326.25 ) return "NW";
  if (bearing >=  326.25 && bearing < 348.75 ) return "NNW";
  if (bearing >=  348.75 && bearing < 360.00 ) return "N";
  else return "N";
}
/*
N    348.75   11.25
NNE   11.25   33.75
NE    33.75   56.25
ENE   56.25   78.75
E     78.75  101.25
ESE  101.25  123.75
SE   123.75  146.25
SSE  146.25  168.75
S    168.75  191.25
SSW  191.25  213.75
SW   213.75  236.25
WSW  236.25  258.75
W    258.75  281.25
WNW  281.25  303.75
NW   303.75  326.25
NNW  326.25  348.75
*/

double NmeaParser::getLong(void) {
  parserState_unset(HAVE_LONG);
  return ((double)longitude)/NMEA_RMC_LONG_PRECISION;
}

char NmeaParser::getLongDir() {
	return longDir;
}

String NmeaParser::getLongitude(boolean majState) {
	if (majState) parserState_unset(HAVE_LONG);
	double f_val = ((double)longitude)/NMEA_RMC_LONG_PRECISION;
	char outstr[15];
	dtostrf(f_val,7, 3, outstr);
	String tmp = String(outstr) + " " + String(longDir);
	return tmp;
}

String NmeaParser::getLongDegree(void) {
  parserState_unset(HAVE_LONG);
	double f_val = ((double)longitude)/NMEA_RMC_LONG_PRECISION;
//	char outstr[15];
//	dtostrf(f_val,7, 3, outstr);
//	String tmp = String(outstr) + " " + String(longDir);
  String tmp = DegreesToDegMinSec(f_val);
	tmp +=  " " + String(longDir);
	return tmp;
}

double NmeaParser::getLat(void) {
  parserState_unset(HAVE_LAT);
  return ((double)latitude)/NMEA_RMC_LAT_PRECISION;
}
	
char NmeaParser::getLatDir(void) {
	return latDir;
}

String NmeaParser::getLatitude(boolean majState) {
  if (majState) parserState_unset(HAVE_LAT);
	double f_val = ((double)latitude)/NMEA_RMC_LAT_PRECISION;
	char outstr[15];
	dtostrf(f_val,7, 3, outstr);
	String tmp = String(outstr) + " " + String(latDir);
	return tmp;	
}

String NmeaParser::getLatDegree(void) {
  parserState_unset(HAVE_LAT);
	double f_val = ((double)latitude)/NMEA_RMC_LAT_PRECISION;
//	char outstr[15];
//	dtostrf(f_val,7, 3, outstr);
//	String tmp = String(outstr) + " " + String(latDir);
  String tmp = DegreesToDegMinSec(f_val);
	tmp += " " + String(latDir);
	return tmp;	
}

String NmeaParser::DegreesToDegMinSec(double x)
{
/*  int deg=x;
  float minutesRemainder = abs(x - deg) * 60;
  int arcMinutes = minutesRemainder;
  float arcSeconds = (minutesRemainder - arcMinutes) * 60;
#ifdef NMEAPARSER_DEBUG
  SerialPort.print("Coordonnée : ");
  SerialPort.print(deg);
	SerialPort.print("*");
  SerialPort.print(arcMinutes);
	SerialPort.print("'");
  SerialPort.print(arcSeconds,4);
	SerialPort.print('"');
  SerialPort.println();
#endif //NMEAPARSER_DEBUG*/

	DUMP(x);
	char outchar[15];
		
	dtostrf(x,7, 3, outchar);
	DUMP(outchar);
	String outstr = String(outchar);
	DUMP(outstr);
	int pos = outstr.indexOf('.');
//	String tmpString = outstr.substring(0,pos-1);
	DUMP(pos);
	
	String chaineSansEspace = "";
	unsigned int lastStringLength = outstr.length();
	
	for(int i=0; i < lastStringLength; i++) {
    if(outstr[i] == '.') break;
    if(outstr[i] != ' ') chaineSansEspace += outstr[i];
	}
	
	DUMP(chaineSansEspace);
	String tmpResult = chaineSansEspace + "*";
	
	String tmpString = "0" + outstr.substring(pos);
	DUMP(tmpString);

	double tmpdouble = tmpString.toDouble();
/*	int deg = x - tmpdouble;
	DUMP(deg);
	String tmpStr = String(deg) + "*";
	*/

/*  float tmpfloat = round(deg*1000);
	int tmpint   = tmpfloat / 1000;
	String tmpStr = String(tmpint) + "*";*/
	
/*  tmpfloat = round(arcMinutes*100);
	tmpint   = tmpfloat / 100;
	tmpStr += String(tmpint) + "'";*/
  float minutesRemainder = tmpdouble * 100;
  int arcMinutes = minutesRemainder;
  float arcSeconds = (minutesRemainder - arcMinutes) * 100;

	tmpResult += String(arcMinutes) + ".";
	
/*  tmpfloat = round(arcSeconds*100);
	tmpint   = tmpfloat / 100;
	tmpStr += String(tmpint) + "''";*/
	int tmpint = arcSeconds;
	tmpResult += String(tmpint); // + "''";	
	
#ifdef NMEAPARSER_DEBUG
  SerialPort.print("Coordonnée : ");
  SerialPort.println(tmpResult);
#endif
	
	return tmpResult;
}