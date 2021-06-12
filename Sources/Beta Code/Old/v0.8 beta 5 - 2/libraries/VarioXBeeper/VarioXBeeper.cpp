/* VarioXBeeper -- Gestion des parametres du son
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
/*                           VarioXBeeper                                        */
/*                                                                               */
/*  version    Date     Description                                              */
/*    1.0    27/06/20                                                            */
/*    1.0.1  27/09/20   Modif valeur de retour getFrenquence                     */
/*                                                                               */
/*********************************************************************************/
/*                                                                               */
/*  calcul la frequence, le cycle et le duty                                     */
/*  compatible avec les données du simulateur xctracer                           */
/*                                                                               */
/*********************************************************************************/

#include <Arduino.h>

#include <HardwareConfig.h>
#include <DebugConfig.h>

#include <VarioSettings.h>
#include <VarioXBeeper.h>

#ifdef HAVE_SDCARD
#include <sdcardHAL.h>
#endif

VarioXBeeper varioXBeeper;

#define RL_MAX_CHARS 128

/******************************************************************************/
boolean VarioXBeeper::init(void) 
/******************************************************************************/
{
 		
  return true;
}


/******************************************************************************/
boolean VarioXBeeper::readSDSettings(char *FileName)
/******************************************************************************/
{
#ifdef HAVE_SDCARD
  char character;

#ifdef SDFAT_LIB
  if (myFile.open(FileName, O_RDONLY)) 
	{
#else //SDFAT_LIB
  myFile = SDHAL_SD.open(FileName);
  if (myFile) 
	{
#endif //SDFAT_LIB		
		char line[RL_MAX_CHARS];
		indexTone = 0;
			
		ClimbToneOnThresholdMaj  = false;
		ClimbToneOffThresholdMaj = false;
		SinkToneOnThresholdMaj   = false;
		SinkToneOffThresholdMaj  = false; 
		
		while (myFile.available()) 
		{

			if (readLines(line)) 
			{
			
#ifdef SDCARD_DEBUG
				SerialPort.print("Read line : ");
				SerialPort.println(line);
#endif //SDCARD_DEBUG
			
				decodeLines(line);
			}
		}
 
		maxRecord = indexTone;
		
		indexTone = 0;
		while ((indexTone < maxRecord) && (tone[indexTone].vario <= 0)) indexTone++;
		
		indexToneZero = indexTone;
#ifdef SDCARD_DEBUG
		SerialPort.print("Max record : ");
		SerialPort.println(maxRecord);
		SerialPort.print("Index zero : ");
		SerialPort.println(indexTone);
		SerialPort.print("Vario : ");
		SerialPort.println(tone[indexTone].vario);
#endif //SDCARD_DEBUG
		
    // close the file:
		myFile.close();
		return true;
  } else 
	{
   // if the file didn't open, print an error:
#ifdef SDCARD_DEBUG
   SerialPort.println("error opening settings.txt");
#endif //SDCARD_DEBUG   
   return false;
  }
#else
	return false;
#endif 
}
 
/******************************************************************************/
boolean VarioXBeeper::readLines(char* line) 
/******************************************************************************/
{
	char c = myFile.read();
	char cIndex = 0;
	while (c != '\n' && cIndex < RL_MAX_CHARS) 
	{
		line[cIndex] = c;
		line[cIndex + 1] = '\0';
		cIndex++;
		if (!myFile.available()) break;
		c = myFile.read();
	}
	
	if (cIndex > 0) return true;
	else 						return false;
}

/******************************************************************************/
boolean VarioXBeeper::decodeLines(char* line) 
/******************************************************************************/
{
	if (line[0] == '#') return false;
	
	char tmpstr[50];
	char tmpstr2[50];
	char delimiter[1];

	delimiter[0] = '=';
	get_value(line,0,delimiter,tmpstr);
	
#ifdef SDCARD_DEBUG
    SerialPort.println("-- decodeLines --");
		SerialPort.print("Champs : ");
		SerialPort.println(tmpstr);
#endif //SDCARD_DEBUG
	
	if (String(tmpstr) == "ClimbToneOnThreshold") 
	{   
#ifdef SDCARD_DEBUG
			SerialPort.print("ClimbToneOnThreshold : ");
#endif //SDCARD_DEBUG		
		get_value(line,1,delimiter,tmpstr);
#ifdef SDCARD_DEBUG
		SerialPort.println(tmpstr);
#endif //SDCARD_DEBUG
		ClimbToneOnThreshold = atof(tmpstr);
	} 
	else if (String(tmpstr) == "ClimbToneOffThreshold") 
	{       
#ifdef SDCARD_DEBUG
		SerialPort.print("ClimbToneOffThreshold : ");
#endif //SDCARD_DEBUG		
		get_value(line,1,delimiter,tmpstr);
#ifdef SDCARD_DEBUG
		SerialPort.println(tmpstr);
#endif //SDCARD_DEBUG
		ClimbToneOffThreshold = atof(tmpstr);
	} 
	else if (String(tmpstr) == "SinkToneOnThreshold") 
	{       
#ifdef SDCARD_DEBUG
		SerialPort.print("SinkToneOnThreshold : ");
#endif //SDCARD_DEBUG		
		get_value(line,1,delimiter,tmpstr);
#ifdef SDCARD_DEBUG
		SerialPort.println(tmpstr);
#endif //SDCARD_DEBUG
		SinkToneOnThreshold = atof(tmpstr);
	} 
	else	if (String(tmpstr) == "SinkToneOffThreshold") 
	{       
#ifdef SDCARD_DEBUG
		SerialPort.print("SinkToneOffThreshold : ");
#endif //SDCARD_DEBUG		
		get_value(line,1,delimiter,tmpstr);
#ifdef SDCARD_DEBUG
		SerialPort.println(tmpstr);
#endif //SDCARD_DEBUG
		SinkToneOffThreshold = atof(tmpstr);
	} 
	else if (String(tmpstr) == "tone") 
	{       
#ifdef SDCARD_DEBUG
		SerialPort.print("tone : ");
#endif //SDCARD_DEBUG		

		get_value(line,1,delimiter,tmpstr2);

#ifdef SDCARD_DEBUG
		SerialPort.println(tmpstr2);
#endif //SDCARD_DEBUG		

		delimiter[0] = ',';

		get_value(tmpstr2,0,delimiter,tmpstr);
		tone[indexTone].vario = atof(tmpstr);
		
#ifdef SDCARD_DEBUG
		SerialPort.print("vario = ");
		SerialPort.print(tone[indexTone].vario);
		SerialPort.print(" / ");
		SerialPort.println(tmpstr);
#endif //SDCARD_DEBUG
		
		get_value(tmpstr2,1,delimiter,tmpstr);
		tone[indexTone].frequence = atof(tmpstr);
		
#ifdef SDCARD_DEBUG
		SerialPort.print("frequence = ");
		SerialPort.println(tone[indexTone].frequence);
#endif //SDCARD_DEBUG

		get_value(tmpstr2,2,delimiter,tmpstr);
		tone[indexTone].cycle = atoi(tmpstr);
		
#ifdef SDCARD_DEBUG
		SerialPort.print("cycle = ");
		SerialPort.println(tone[indexTone].cycle);
#endif //SDCARD_DEBUG

		get_value(tmpstr2,3,delimiter,tmpstr);
		tone[indexTone].duty = atoi(tmpstr);
		
#ifdef SDCARD_DEBUG
		SerialPort.print("duty = ");
		SerialPort.println(tone[indexTone].duty);
#endif //SDCARD_DEBUG

		indexTone++;
	}				
}

/******************************************************************************/
void VarioXBeeper::get_value(char * chaine, byte index, char * delimiter, char * get_val)
/******************************************************************************/
{
  // Get the value from a char delimited with comma
  // 123,rftzg,4568
  // get_value(1), will return rftzg

  // char chaine[]= "block0,block1,block2,block3"

  // Initiate the
  byte ind = 0; // Position of the block of data into the chaine
  byte x = 0;   // index of val. Value to retunr.
  byte ic = 0;  // Index Chaine

  // In my comment, take in concideration that index is egal to 0. We need to print the first block : 'block0'
  do // Go trough chaine, one by one
  {
    //Serial.println(chaine[ic]);
    if(chaine[ic]==delimiter[0])
    {
        ind++;                  // next block
    }
    else if(ind == index)        // if ind is egal to index. Index is the block we decide to have printed
    {
        get_val[x] = chaine[ic];  // Store the desired value in val 
        // Serial.print(F("Def")); Serial.println(val[x]);
        x++;
    }
    ic++;                       // Go to next position of chaine
    //Serial.print(F("co")); Serial.println(co);

  }while( ic <= strlen(chaine)); // As long as ic is < than the amount tof caracter in chaine, and then leave the do loop
  get_val[x]='\0';  // Close val with \0
 
  //Serial.print(F("sr:")); Serial.println(get_val);
  //return get_val;
}
 
/******************************************************************************/
void VarioXBeeper::majVarioSettings(boolean *ModifiedValue) 
/******************************************************************************/
{
	 /* <--LOW-BEEP--|------SILENT------|--NEAR-CLIMBING-BEEP--|--CLIMBING-BEEP--> */
/*                 |                  |                      |                   */
/*                SINK              ZERO                   CLIMB                 */

   if ((ClimbToneOnThresholdMaj) && (GnuSettings.CLIMB_THRESHOLD  != ClimbToneOnThreshold)) 
	 {  
     GnuSettings.CLIMB_THRESHOLD  = ClimbToneOnThreshold;
		 *ModifiedValue = true;
	 }
		 
   if ((ClimbToneOffThresholdMaj) && (GnuSettings.ZERO_THRESHOLD != ClimbToneOffThreshold))
	 {
		 GnuSettings.ZERO_THRESHOLD	 	= ClimbToneOffThreshold;
		 *ModifiedValue = true;
	 }
		 
	 if ((SinkToneOnThresholdMaj) && (GnuSettings.SINK_THRESHOLD  != SinkToneOnThreshold)) 
	 {
		 GnuSettings.SINK_THRESHOLD   = SinkToneOnThreshold;
		 *ModifiedValue = true;
	 }
}

/*********************************************/
uint16_t VarioXBeeper::getFrequence(float vario) 
/*********************************************/
{
	float m, p;
  uint16_t retour;

#ifdef SDCARD_DEBUG
	SerialPort.print("calcul frequence -- vario = ");
	SerialPort.println(vario);
#endif //SDCARD_DEBUG
	
	if (maxRecord == 0)
	{
		//Pas de fichier de configuration du son 
		//Frequence calculer avec la methode d'origine
		
		retour = 0;
		return retour;
	}
	
	indexTone = 0;
	
	if (vario > 0) {
		for(int i=indexToneZero; i<maxRecord;i++) 
			if (tone[i].vario > vario) {
				indexTone = i;
				break;
			}
	} else {
		for(int i=indexToneZero; i>0;i--) 
			if (tone[i].vario < vario) {
				indexTone = i;
				break;
			}
	}
	
#ifdef SDCARD_DEBUG
	SerialPort.print("calcul index = ");
	SerialPort.println(indexTone);
	SerialPort.print("calcul tone vario = ");
	SerialPort.println(tone[indexTone].vario);
#endif //SDCARD_DEBUG
	
//	while ((indexTone < maxRecord) && (tone[indexTone].vario < vario)) indexTone++;
//	while ((indexTone > 0) && (tone[indexTone].vario > vario)) indexTone--;
	
	//213 / 1.51 = 141.059          
	
	if (vario > 0) {
	  m = (tone[indexTone].frequence - tone[indexTone-1].frequence) / (tone[indexTone].vario - tone[indexTone-1].vario);
	  p = tone[indexTone-1].frequence - (tone[indexTone-1].vario * m);
	} else {
	  m = (tone[indexTone+1].frequence - tone[indexTone].frequence) / (tone[indexTone+1].vario - tone[indexTone].vario);
	  p = tone[indexTone].frequence - (tone[indexTone].vario * m);
	}
	
	retour = (m*vario)+p;
	
#ifdef SDCARD_DEBUG
	SerialPort.print("calcul frequence = ");
	SerialPort.println(retour);
#endif //SDCARD_DEBUG

	return retour;
}

/*********************************************/
int VarioXBeeper::getCycle(float vario) 
/*********************************************/
{
	float m, p;
	int retour;

#ifdef SDCARD_DEBUG
	SerialPort.print("calcul cycle -- vario = ");
	SerialPort.println(vario);
#endif //SDCARD_DEBUG

	if (maxRecord == 0)
	{
		//Pas de fichier de configuration du son 
		//cycle calculer avec la methode d'origine
		
		retour = 100;
		return retour;
	}
	
	indexTone = 0;
	
	if (vario > 0) {
		for(int i=indexToneZero; i<maxRecord;i++) 
			if (tone[i].vario > vario) {
				indexTone = i;
				break;
			}
	} else {
		for(int i=indexToneZero; i>0;i--) 
			if (tone[i].vario < vario) {
				indexTone = i;
				break;
			}
	}
	
#ifdef SDCARD_DEBUG
	SerialPort.print("calcul index = ");
	SerialPort.println(indexTone);
	SerialPort.print("calcul tone vario = ");
	SerialPort.println(tone[indexTone].vario);
#endif //SDCARD_DEBUG
	
//	while ((indexTone < maxRecord) && (tone[indexTone].vario < vario)) indexTone++;
//	while ((indexTone > 0) && (tone[indexTone].vario > vario)) indexTone--;
	
	//213 / 1.51 = 141.059          
	
	if (vario > 0) {
	  m = (tone[indexTone].cycle - tone[indexTone-1].cycle) / (tone[indexTone].vario - tone[indexTone-1].vario);
	  p = tone[indexTone-1].cycle - (tone[indexTone-1].vario * m);
	} else {
	  m = (tone[indexTone+1].cycle - tone[indexTone].cycle) / (tone[indexTone+1].vario - tone[indexTone].vario);
	  p = tone[indexTone].cycle - (tone[indexTone].vario * m);
	}
	
	retour = (m*vario)+p;
#ifdef SDCARD_DEBUG
	SerialPort.print("calcul cycle = ");
	SerialPort.println(retour);
#endif //SDCARD_DEBUG

	return retour;
}

/*********************************************/
int VarioXBeeper::getDuty(float vario) 
/*********************************************/
{
	float m, p;
	int retour;

#ifdef SDCARD_DEBUG
	SerialPort.print("calcul duty -- vario = ");
	SerialPort.println(vario);
#endif //SDCARD_DEBUG

	if (maxRecord == 0)
	{
		//Pas de fichier de configuration du son 
		//duty calculer avec la methode d'origine
		
		retour = 100;
		return retour;
	}
	
	indexTone = 0;
	
	if (vario > 0) {
		for(int i=indexToneZero; i<maxRecord;i++) 
			if (tone[i].vario > vario) {
				indexTone = i;
				break;
			}
	} else {
		for(int i=indexToneZero; i>0;i--) 
			if (tone[i].vario < vario) {
				indexTone = i;
				break;
			}
	}
	
#ifdef SDCARD_DEBUG
	SerialPort.print("calcul index = ");
	SerialPort.println(indexTone);
	SerialPort.print("calcul tone vario = ");
	SerialPort.println(tone[indexTone].vario);
#endif //SDCARD_DEBUG
	
//	while ((indexTone < maxRecord) && (tone[indexTone].vario < vario)) indexTone++;
//	while ((indexTone > 0) && (tone[indexTone].vario > vario)) indexTone--;
	
	//213 / 1.51 = 141.059          
	
	if (vario > 0) {
	  m = (tone[indexTone].duty - tone[indexTone-1].duty) / (tone[indexTone].vario - tone[indexTone-1].vario);
	  p = tone[indexTone-1].duty - (tone[indexTone-1].vario * m);
	} else {
	  m = (tone[indexTone+1].duty - tone[indexTone].duty) / (tone[indexTone+1].vario - tone[indexTone].vario);
	  p = tone[indexTone].duty - (tone[indexTone].vario * m);
	}
	
	retour = (m*vario)+p;
#ifdef SDCARD_DEBUG
	SerialPort.print("calcul duty = ");
	SerialPort.println(retour);
#endif //SDCARD_DEBUG

  return retour;
}