/* VarioLanguage -- 
 *
 * Copyright 2020 MichelPa / Jpg63
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

/* 
 *********************************************************************************
 *                                                                               *
 *                           VarioLanguage                                       *
 *                                                                               *
 *  version    Date     Description                                              *
 *    1.0    11/04/20                                                            *
 *                                                                               *
 *********************************************************************************
 */

#include <HardwareConfig.h>
#include <DebugConfig.h>

#ifdef DATA_DEBUG
#define ARDUINOTRACE_ENABLE 1
#else
#define ARDUINOTRACE_ENABLE 0
#endif

#define ARDUINOTRACE_SERIAL SerialPort
#include <ArduinoTrace.h>

#include <sdcardHAL.h>
//#include <VarioSettings.h>

#include <VarioLanguage.h>

#include <ArduinoJson.h>

#include <varioscreenGxEPD.h>

VarioLanguage varioLanguage;

#define PARAMS_VERSION_LANGUE "1.0"

#define DEFAULT_TITRE_TIME "Heure"
#define DEFAULT_TITRE_TDV "Temps de vol"
#define DEFAULT_TITRE_ALTI "Alti"
#define DEFAULT_TITRE_AGL "Alti Sol"
#define DEFAULT_TITRE_VARIO "Vario"
#define DEFAULT_TITRE_SPEED "Vitesse"
#define DEFAULT_TITRE_FINESSE "Finesse"
#define DEFAULT_TITRE_TCHUTE "T Chute"
#define DEFAULT_TITRE_CAP "Cap"
#define DEFAULT_TITRE_LAT "Lat"
#define DEFAULT_TITRE_LONG "Long"
#define DEFAULT_TITRE_COMPAS "Compas"
#define DEFAULT_TITRE_STAT "Statistique"
#define DEFAULT_TITRE_DATE "Date"
#define DEFAULT_TITRE_HEURE "Heure"
#define DEFAULT_TITRE_DUREE "Duree"
#define DEFAULT_TITRE_VITESSE "Vitesse"
#define DEFAULT_TITRE_CONNECT "Connection"
#define DEFAULT_TITRE_CONNECTA "Connection a "
#define DEFAULT_TITRE_DEMAR "Demarrage"
#define DEFAULT_TITRE_REDEMAR "Redemarrage"
#define DEFAULT_TITRE_ENCOURS "en cours"
#define DEFAULT_TITRE_CALIBR "Calibration"
#define DEFAULT_TITRE_VEILLE "En veille"

//****************************************************************************************************************************
void VarioLanguage::init(uint8_t language)
//****************************************************************************************************************************
{
	char tmpchar[20] = "/gnuvario";

	switch (language)
	{
	case 0:
		strcat(tmpchar, "FR");
		break;
	case 1:
		strcat(tmpchar, "EN");
		break;
	default:
		strcat(tmpchar, "FR");
		break;
	}
	strcat(tmpchar, ".jso");
	loadConfigurationLangue(tmpchar);
}

//**********************************************************
void VarioLanguage::loadConfigurationLangue(char *filename)
{
	//**********************************************************
	// Open file for reading
	boolean MajFileParams = false;
#ifdef SDFAT_LIB
	SdFile file;
	if (!file.open(filename, O_RDONLY))
	{
#else
	File file = SDHAL_SD.open(filename);
	if (!file)
	{
#endif
		SerialPort.println(F("Failed to read file"));

		TITRE_TAB[TITRE_TIME] = DEFAULT_TITRE_TIME;
		TITRE_TAB[TITRE_TDV] = DEFAULT_TITRE_TDV;
		TITRE_TAB[TITRE_ALTI] = DEFAULT_TITRE_ALTI;
		TITRE_TAB[TITRE_VARIO] = DEFAULT_TITRE_VARIO;
		TITRE_TAB[TITRE_SPEED] = DEFAULT_TITRE_SPEED;
		TITRE_TAB[TITRE_FINESSE] = DEFAULT_TITRE_FINESSE;
		TITRE_TAB[TITRE_TCHUTE] = DEFAULT_TITRE_TCHUTE;
		TITRE_TAB[TITRE_CAP] = DEFAULT_TITRE_CAP;
		TITRE_TAB[TITRE_LAT] = DEFAULT_TITRE_LAT;
		TITRE_TAB[TITRE_LONG] = DEFAULT_TITRE_LONG;
		TITRE_TAB[TITRE_COMPAS] = DEFAULT_TITRE_COMPAS;
		TITRE_TAB[TITRE_STAT] = DEFAULT_TITRE_STAT;
		TITRE_TAB[TITRE_DATE] = DEFAULT_TITRE_DATE;
		TITRE_TAB[TITRE_HEURE] = DEFAULT_TITRE_HEURE;
		TITRE_TAB[TITRE_DUREE] = DEFAULT_TITRE_DUREE;
		TITRE_TAB[TITRE_VITESSE] = DEFAULT_TITRE_VITESSE;
		TITRE_TAB[TITRE_CONNECT] = DEFAULT_TITRE_CONNECT;
		TITRE_TAB[TITRE_CONNECTA] = DEFAULT_TITRE_CONNECTA;
		TITRE_TAB[TITRE_DEMAR] = DEFAULT_TITRE_DEMAR;
		TITRE_TAB[TITRE_REDEMAR] = DEFAULT_TITRE_REDEMAR;
		TITRE_TAB[TITRE_ENCOURS] = DEFAULT_TITRE_ENCOURS;
		TITRE_TAB[TITRE_CALIBR] = DEFAULT_TITRE_CALIBR;
		TITRE_TAB[TITRE_VEILLE] = DEFAULT_TITRE_VEILLE;

		return;
	}

	const size_t capacity = JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(4) + 2 * JSON_OBJECT_SIZE(12) + 460;
	//JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(4) + JSON_OBJECT_SIZE(10) + JSON_OBJECT_SIZE(12)+100;
	DynamicJsonDocument doc(capacity);

#ifdef SDCARD_DEBUG
	SerialPort.println("deserialisation");
#endif

	// Deserialize the JSON document
	DeserializationError error = deserializeJson(doc, file);
	if (error)
	{
		SerialPort.println(F("Failed to read file, using default configuration"));

		TITRE_TAB[TITRE_TIME] = DEFAULT_TITRE_TIME;
		TITRE_TAB[TITRE_TDV] = DEFAULT_TITRE_TDV;
		TITRE_TAB[TITRE_ALTI] = DEFAULT_TITRE_ALTI;
		TITRE_TAB[TITRE_VARIO] = DEFAULT_TITRE_VARIO;
		TITRE_TAB[TITRE_SPEED] = DEFAULT_TITRE_SPEED;
		TITRE_TAB[TITRE_FINESSE] = DEFAULT_TITRE_FINESSE;
		TITRE_TAB[TITRE_TCHUTE] = DEFAULT_TITRE_TCHUTE;
		TITRE_TAB[TITRE_CAP] = DEFAULT_TITRE_CAP;
		TITRE_TAB[TITRE_LAT] = DEFAULT_TITRE_LAT;
		TITRE_TAB[TITRE_LONG] = DEFAULT_TITRE_LONG;
		TITRE_TAB[TITRE_COMPAS] = DEFAULT_TITRE_COMPAS;
		TITRE_TAB[TITRE_STAT] = DEFAULT_TITRE_STAT;
		TITRE_TAB[TITRE_DATE] = DEFAULT_TITRE_DATE;
		TITRE_TAB[TITRE_HEURE] = DEFAULT_TITRE_HEURE;
		TITRE_TAB[TITRE_DUREE] = DEFAULT_TITRE_DUREE;
		TITRE_TAB[TITRE_VITESSE] = DEFAULT_TITRE_VITESSE;
		TITRE_TAB[TITRE_CONNECT] = DEFAULT_TITRE_CONNECT;
		TITRE_TAB[TITRE_CONNECTA] = DEFAULT_TITRE_CONNECTA;
		TITRE_TAB[TITRE_DEMAR] = DEFAULT_TITRE_DEMAR;
		TITRE_TAB[TITRE_REDEMAR] = DEFAULT_TITRE_REDEMAR;
		TITRE_TAB[TITRE_ENCOURS] = DEFAULT_TITRE_ENCOURS;
		TITRE_TAB[TITRE_CALIBR] = DEFAULT_TITRE_CALIBR;
		TITRE_TAB[TITRE_VEILLE] = DEFAULT_TITRE_VEILLE;

		return;
	}

	String tmpValueString;

#ifdef SDCARD_DEBUG
	SerialPort.println("Systeme : ");
#endif

	const char *GnuvarioE_version_langue = doc["gnuvarioe"]["version"]; // "1.0"
	if (strcmp(GnuvarioE_version_langue, PARAMS_VERSION_LANGUE) != 0)
		MajFileParams = true;

		//*****    SYSTEME *****

#ifdef SDCARD_DEBUG
	SerialPort.println("****** Titre *******");
#endif

	JsonObject Titre = doc["titre"];

	if (Titre.containsKey("TIME"))
	{
		const char *Titre_TIME = Titre["TIME"];
#ifdef SDCARD_DEBUG
		SerialPort.print("Titre_TIME : ");
		SerialPort.println(Titre_TIME);
#endif
		tmpValueString = String(Titre_TIME);

#ifdef SDCARD_DEBUG
		SerialPort.print("tmpValueString : ");
		SerialPort.println(tmpValueString);
#endif

#ifdef SDCARD_DEBUG
		SerialPort.print("Json Recup - ");
#endif
	}
	else
	{
		tmpValueString = DEFAULT_TITRE_TIME;
		MajFileParams = true;
#ifdef SDCARD_DEBUG
		SerialPort.print("Defaut Recup - ");
#endif
	}
	TITRE_TAB[TITRE_TIME] = tmpValueString.substring(0, MAX_CAR_TITRE_TIME);
#ifdef SDCARD_DEBUG
	SerialPort.print("Index : ");
	SerialPort.print(TITRE_TIME);
	SerialPort.print(" - Max car : ");
	SerialPort.println(MAX_CAR_TITRE_TIME);
	SerialPort.print("TITRE_TIME : ");
	SerialPort.println(TITRE_TAB[TITRE_TIME]);
#endif

	if (Titre.containsKey("TDV"))
	{
		const char *Titre_TDV = Titre["TDV"];

		tmpValueString = String(Titre_TDV);

#ifdef SDCARD_DEBUG
		SerialPort.print("tmpValueString : ");
		SerialPort.println(tmpValueString);
#endif

#ifdef SDCARD_DEBUG
		SerialPort.print("Json Recup - ");
#endif
	}
	else
	{
		tmpValueString = DEFAULT_TITRE_TDV;
		MajFileParams = true;
#ifdef SDCARD_DEBUG
		SerialPort.print("Defaut Recup - ");
#endif
	}

	TITRE_TAB[TITRE_TDV] = tmpValueString.substring(0, MAX_CAR_TITRE_TDV);
#ifdef SDCARD_DEBUG
	SerialPort.print("Index : ");
	SerialPort.print(TITRE_TDV);
	SerialPort.print(" - Max car : ");
	SerialPort.println(MAX_CAR_TITRE_TDV);
	SerialPort.print("TITRE_TDV : ");
	SerialPort.println(TITRE_TAB[TITRE_TDV]);
#endif

	if (Titre.containsKey("SPEED"))
	{
		const char *Titre_SPEED = Titre["SPEED"];
#ifdef SDCARD_DEBUG
		SerialPort.print("Titre_SPEED : ");
		SerialPort.println(Titre_SPEED);
#endif
		tmpValueString = String(Titre_SPEED);

#ifdef SDCARD_DEBUG
		SerialPort.print("tmpValueString : ");
		SerialPort.println(tmpValueString);
#endif

#ifdef SDCARD_DEBUG
		SerialPort.print("Json Recup - ");
#endif
	}
	else
	{
		tmpValueString = DEFAULT_TITRE_SPEED;
		MajFileParams = true;
#ifdef SDCARD_DEBUG
		SerialPort.print("Defaut Recup - ");
#endif
	}
	TITRE_TAB[TITRE_SPEED] = tmpValueString.substring(0, MAX_CAR_TITRE_SPEED);
#ifdef SDCARD_DEBUG
	SerialPort.print("Index : ");
	SerialPort.print(TITRE_SPEED);
	SerialPort.print(" - Max car : ");
	SerialPort.println(MAX_CAR_TITRE_SPEED);
	SerialPort.print("TITRE_SPEED : ");
	SerialPort.println(TITRE_TAB[TITRE_SPEED]);
#endif

	if (Titre.containsKey("ALTI"))
	{
		const char *Titre_ALTI = Titre["ALTI"];
#ifdef SDCARD_DEBUG
		SerialPort.print("Titre_ALTI : ");
		SerialPort.println(Titre_ALTI);
#endif
		tmpValueString = String(Titre_ALTI);

#ifdef SDCARD_DEBUG
		SerialPort.print("tmpValueString : ");
		SerialPort.println(tmpValueString);
#endif

#ifdef SDCARD_DEBUG
		SerialPort.print("Json Recup - ");
#endif
	}
	else
	{
		tmpValueString = DEFAULT_TITRE_ALTI;
		MajFileParams = true;
#ifdef SDCARD_DEBUG
		SerialPort.print("Defaut Recup - ");
#endif
	}
	TITRE_TAB[TITRE_ALTI] = tmpValueString.substring(0, MAX_CAR_TITRE_ALTI);
#ifdef SDCARD_DEBUG
	SerialPort.print("Index : ");
	SerialPort.print(TITRE_ALTI);
	SerialPort.print(" - Max car : ");
	SerialPort.println(MAX_CAR_TITRE_ALTI);
	SerialPort.print("TITRE_ALTI : ");
	SerialPort.println(TITRE_TAB[TITRE_ALTI]);
#endif

	if (Titre.containsKey("AGL"))
	{
		const char *Titre_AGL = Titre["AGL"];
#ifdef SDCARD_DEBUG
		SerialPort.print("Titre_AGL : ");
		SerialPort.println(Titre_AGL);
#endif
		tmpValueString = String(Titre_AGL);

#ifdef SDCARD_DEBUG
		SerialPort.print("tmpValueString : ");
		SerialPort.println(tmpValueString);
#endif

#ifdef SDCARD_DEBUG
		SerialPort.print("Json Recup - ");
#endif
	}
	else
	{
		tmpValueString = DEFAULT_TITRE_AGL;
		MajFileParams = true;
#ifdef SDCARD_DEBUG
		SerialPort.print("Defaut Recup - ");
#endif
	}
	TITRE_TAB[TITRE_AGL] = tmpValueString.substring(0, MAX_CAR_TITRE_AGL);
#ifdef SDCARD_DEBUG
	SerialPort.print("Index : ");
	SerialPort.print(TITRE_AGL);
	SerialPort.print(" - Max car : ");
	SerialPort.println(MAX_CAR_TITRE_AGL);
	SerialPort.print("TITRE_AGL : ");
	SerialPort.println(TITRE_TAB[TITRE_AGL]);
#endif

	if (Titre.containsKey("VARIO"))
	{
		const char *Titre_VARIO = Titre["VARIO"];
#ifdef SDCARD_DEBUG
		SerialPort.print("Titre_VARIO : ");
		SerialPort.println(Titre_VARIO);
#endif
		tmpValueString = String(Titre_VARIO);

#ifdef SDCARD_DEBUG
		SerialPort.print("tmpValueString : ");
		SerialPort.println(tmpValueString);
#endif

#ifdef SDCARD_DEBUG
		SerialPort.print("Json Recup - ");
#endif
	}
	else
	{
		tmpValueString = DEFAULT_TITRE_VARIO;
		MajFileParams = true;
#ifdef SDCARD_DEBUG
		SerialPort.print("Defaut Recup - ");
#endif
	}
	TITRE_TAB[TITRE_VARIO] = tmpValueString.substring(0, MAX_CAR_TITRE_VARIO);
#ifdef SDCARD_DEBUG
	SerialPort.print("Index : ");
	SerialPort.print(TITRE_VARIO);
	SerialPort.print(" - Max car : ");
	SerialPort.println(MAX_CAR_TITRE_VARIO);
	SerialPort.print("TITRE_VARIO : ");
	SerialPort.println(TITRE_TAB[TITRE_VARIO]);
#endif

	if (Titre.containsKey("FINESSE"))
	{
		const char *Titre_FINESSE = Titre["FINESSE"];
#ifdef SDCARD_DEBUG
		SerialPort.print("Titre_FINESSE : ");
		SerialPort.println(Titre_FINESSE);
#endif
		tmpValueString = String(Titre_FINESSE);

#ifdef SDCARD_DEBUG
		SerialPort.print("tmpValueString : ");
		SerialPort.println(tmpValueString);
#endif

#ifdef SDCARD_DEBUG
		SerialPort.print("Json Recup - ");
#endif
	}
	else
	{
		tmpValueString = DEFAULT_TITRE_FINESSE;
		MajFileParams = true;
#ifdef SDCARD_DEBUG
		SerialPort.print("Defaut Recup - ");
#endif
	}
	TITRE_TAB[TITRE_FINESSE] = tmpValueString.substring(0, MAX_CAR_TITRE_FINESSE);
#ifdef SDCARD_DEBUG
	SerialPort.print("Index : ");
	SerialPort.print(TITRE_FINESSE);
	SerialPort.print(" - Max car : ");
	SerialPort.println(MAX_CAR_TITRE_FINESSE);
	SerialPort.print("TITRE_FINESSE : ");
	SerialPort.println(TITRE_TAB[TITRE_FINESSE]);
#endif

	if (Titre.containsKey("TCHUTE"))
	{
		const char *Titre_TCHUTE = Titre["TCHUTE"];
#ifdef SDCARD_DEBUG
		SerialPort.print("Titre_TCHUTE : ");
		SerialPort.println(Titre_TCHUTE);
#endif
		tmpValueString = String(Titre_TCHUTE);

#ifdef SDCARD_DEBUG
		SerialPort.print("tmpValueString : ");
		SerialPort.println(tmpValueString);
#endif

#ifdef SDCARD_DEBUG
		SerialPort.print("Json Recup - ");
#endif
	}
	else
	{
		tmpValueString = DEFAULT_TITRE_TCHUTE;
		MajFileParams = true;
#ifdef SDCARD_DEBUG
		SerialPort.print("Defaut Recup - ");
#endif
	}
	TITRE_TAB[TITRE_TCHUTE] = tmpValueString.substring(0, MAX_CAR_TITRE_TCHUTE);
#ifdef SDCARD_DEBUG
	SerialPort.print("Index : ");
	SerialPort.print(TITRE_TCHUTE);
	SerialPort.print(" - Max car : ");
	SerialPort.println(MAX_CAR_TITRE_TCHUTE);
	SerialPort.print("TITRE_TCHUTE : ");
	SerialPort.println(TITRE_TAB[TITRE_TCHUTE]);
#endif

	if (Titre.containsKey("CAP"))
	{
		const char *Titre_CAP = Titre["CAP"];
#ifdef SDCARD_DEBUG
		SerialPort.print("Titre_CAP : ");
		SerialPort.println(Titre_CAP);
#endif
		tmpValueString = String(Titre_CAP);

#ifdef SDCARD_DEBUG
		SerialPort.print("tmpValueString : ");
		SerialPort.println(tmpValueString);
#endif

#ifdef SDCARD_DEBUG
		SerialPort.print("Json Recup - ");
#endif
	}
	else
	{
		tmpValueString = DEFAULT_TITRE_CAP;
		MajFileParams = true;
#ifdef SDCARD_DEBUG
		SerialPort.print("Defaut Recup - ");
#endif
	}
	TITRE_TAB[TITRE_CAP] = tmpValueString.substring(0, MAX_CAR_TITRE_CAP);
#ifdef SDCARD_DEBUG
	SerialPort.print("Index : ");
	SerialPort.print(TITRE_CAP);
	SerialPort.print(" - Max car : ");
	SerialPort.println(MAX_CAR_TITRE_CAP);
	SerialPort.print("TITRE_CAP : ");
	SerialPort.println(TITRE_TAB[TITRE_CAP]);
#endif

	if (Titre.containsKey("LAT"))
	{
		const char *Titre_LAT = Titre["LAT"];
#ifdef SDCARD_DEBUG
		SerialPort.print("Titre_LAT : ");
		SerialPort.println(Titre_LAT);
#endif
		tmpValueString = String(Titre_LAT);

#ifdef SDCARD_DEBUG
		SerialPort.print("tmpValueString : ");
		SerialPort.println(tmpValueString);
#endif

#ifdef SDCARD_DEBUG
		SerialPort.print("Json Recup - ");
#endif
	}
	else
	{
		tmpValueString = DEFAULT_TITRE_LAT;
		MajFileParams = true;
#ifdef SDCARD_DEBUG
		SerialPort.print("Defaut Recup - ");
#endif
	}
	TITRE_TAB[TITRE_LAT] = tmpValueString.substring(0, MAX_CAR_TITRE_LAT);
#ifdef SDCARD_DEBUG
	SerialPort.print("Index : ");
	SerialPort.print(TITRE_LAT);
	SerialPort.print(" - Max car : ");
	SerialPort.println(MAX_CAR_TITRE_LAT);
	SerialPort.print("TITRE_LAT : ");
	SerialPort.println(TITRE_TAB[TITRE_LAT]);
#endif

	if (Titre.containsKey("LONG"))
	{
		const char *Titre_LONG = Titre["LONG"];
#ifdef SDCARD_DEBUG
		SerialPort.print("Titre_LONG : ");
		SerialPort.println(Titre_LONG);
#endif
		tmpValueString = String(Titre_LONG);

#ifdef SDCARD_DEBUG
		SerialPort.print("tmpValueString : ");
		SerialPort.println(tmpValueString);
#endif

#ifdef SDCARD_DEBUG
		SerialPort.print("Json Recup - ");
#endif
	}
	else
	{
		tmpValueString = DEFAULT_TITRE_LONG;
		MajFileParams = true;
#ifdef SDCARD_DEBUG
		SerialPort.print("Defaut Recup - ");
#endif
	}
	TITRE_TAB[TITRE_LONG] = tmpValueString.substring(0, MAX_CAR_TITRE_LONG);
#ifdef SDCARD_DEBUG
	SerialPort.print("Index : ");
	SerialPort.print(TITRE_LONG);
	SerialPort.print(" - Max car : ");
	SerialPort.println(MAX_CAR_TITRE_LONG);
	SerialPort.print("TITRE_LONG : ");
	SerialPort.println(TITRE_TAB[TITRE_LONG]);
#endif

	if (Titre.containsKey("COMPAS"))
	{
		const char *Titre_COMPAS = Titre["COMPAS"];
#ifdef SDCARD_DEBUG
		SerialPort.print("Titre_COMPAS : ");
		SerialPort.println(Titre_COMPAS);
#endif
		tmpValueString = String(Titre_COMPAS);

#ifdef SDCARD_DEBUG
		SerialPort.print("tmpValueString : ");
		SerialPort.println(tmpValueString);
#endif

#ifdef SDCARD_DEBUG
		SerialPort.print("Json Recup - ");
#endif
	}
	else
	{
		tmpValueString = DEFAULT_TITRE_COMPAS;
		MajFileParams = true;
#ifdef SDCARD_DEBUG
		SerialPort.print("Defaut Recup - ");
#endif
	}
	TITRE_TAB[TITRE_COMPAS] = tmpValueString.substring(0, MAX_CAR_TITRE_COMPAS);
#ifdef SDCARD_DEBUG
	SerialPort.print("Index : ");
	SerialPort.print(TITRE_COMPAS);
	SerialPort.print(" - Max car : ");
	SerialPort.println(MAX_CAR_TITRE_COMPAS);
	SerialPort.print("TITRE_COMPAS : ");
	SerialPort.println(TITRE_TAB[TITRE_COMPAS]);
#endif

	//*****    MESSAGE *****

#ifdef SDCARD_DEBUG
	SerialPort.println("****** Message *******");
#endif

	JsonObject Message = doc["message"];

	if (Message.containsKey("STAT"))
	{
		const char *Message_STAT = Message["STAT"];
#ifdef SDCARD_DEBUG
		SerialPort.print("Message_STAT : ");
		SerialPort.println(Message_STAT);
#endif
		tmpValueString = String(Message_STAT);

#ifdef SDCARD_DEBUG
		SerialPort.print("tmpValueString : ");
		SerialPort.println(tmpValueString);
#endif

#ifdef SDCARD_DEBUG
		SerialPort.print("Json Recup - ");
#endif
	}
	else
	{
		tmpValueString = DEFAULT_TITRE_STAT;
		MajFileParams = true;
#ifdef SDCARD_DEBUG
		SerialPort.print("Defaut Recup - ");
#endif
	}
	TITRE_TAB[TITRE_STAT] = tmpValueString.substring(0, MAX_CAR_TITRE_STAT);
#ifdef SDCARD_DEBUG
	SerialPort.print("Index : ");
	SerialPort.print(TITRE_STAT);
	SerialPort.print(" - Max car : ");
	SerialPort.println(MAX_CAR_TITRE_STAT);
	SerialPort.print("TITRE_STAT : ");
	SerialPort.println(TITRE_TAB[TITRE_STAT]);
#endif

	if (Message.containsKey("DATE"))
	{
		const char *Message_DATE = Message["DATE"];
#ifdef SDCARD_DEBUG
		SerialPort.print("Message_DATE : ");
		SerialPort.println(Message_DATE);
#endif
		tmpValueString = String(Message_DATE);

#ifdef SDCARD_DEBUG
		SerialPort.print("tmpValueString : ");
		SerialPort.println(tmpValueString);
#endif

#ifdef SDCARD_DEBUG
		SerialPort.print("Json Recup - ");
#endif
	}
	else
	{
		tmpValueString = DEFAULT_TITRE_DATE;
		MajFileParams = true;
#ifdef SDCARD_DEBUG
		SerialPort.print("Defaut Recup - ");
#endif
	}
	TITRE_TAB[TITRE_DATE] = tmpValueString.substring(0, MAX_CAR_TITRE_DATE);
#ifdef SDCARD_DEBUG
	SerialPort.print("Index : ");
	SerialPort.print(TITRE_DATE);
	SerialPort.print(" - Max car : ");
	SerialPort.println(MAX_CAR_TITRE_DATE);
	SerialPort.print("TITRE_DATE : ");
	SerialPort.println(TITRE_TAB[TITRE_DATE]);
#endif

	if (Message.containsKey("HEURE"))
	{
		const char *Message_HEURE = Message["HEURE"];
#ifdef SDCARD_DEBUG
		SerialPort.print("Message_HEURE : ");
		SerialPort.println(Message_HEURE);
#endif
		tmpValueString = String(Message_HEURE);

#ifdef SDCARD_DEBUG
		SerialPort.print("tmpValueString : ");
		SerialPort.println(tmpValueString);
#endif

#ifdef SDCARD_DEBUG
		SerialPort.print("Json Recup - ");
#endif
	}
	else
	{
		tmpValueString = DEFAULT_TITRE_HEURE;
		MajFileParams = true;
#ifdef SDCARD_DEBUG
		SerialPort.print("Defaut Recup - ");
#endif
	}
	TITRE_TAB[TITRE_HEURE] = tmpValueString.substring(0, MAX_CAR_TITRE_HEURE);
#ifdef SDCARD_DEBUG
	SerialPort.print("Index : ");
	SerialPort.print(TITRE_HEURE);
	SerialPort.print(" - Max car : ");
	SerialPort.println(MAX_CAR_TITRE_HEURE);
	SerialPort.print("TITRE_HEURE : ");
	SerialPort.println(TITRE_TAB[TITRE_HEURE]);
#endif

	if (Message.containsKey("DUREE"))
	{
		const char *Message_DUREE = Message["DUREE"];
#ifdef SDCARD_DEBUG
		SerialPort.print("Message_DUREE : ");
		SerialPort.println(Message_DUREE);
#endif
		tmpValueString = String(Message_DUREE);

#ifdef SDCARD_DEBUG
		SerialPort.print("tmpValueString : ");
		SerialPort.println(tmpValueString);
#endif

#ifdef SDCARD_DEBUG
		SerialPort.print("Json Recup - ");
#endif
	}
	else
	{
		tmpValueString = DEFAULT_TITRE_DUREE;
		MajFileParams = true;
#ifdef SDCARD_DEBUG
		SerialPort.print("Defaut Recup - ");
#endif
	}
	TITRE_TAB[TITRE_DUREE] = tmpValueString.substring(0, MAX_CAR_TITRE_DUREE);
#ifdef SDCARD_DEBUG
	SerialPort.print("Index : ");
	SerialPort.print(TITRE_DUREE);
	SerialPort.print(" - Max car : ");
	SerialPort.println(MAX_CAR_TITRE_DUREE);
	SerialPort.print("TITRE_DUREE : ");
	SerialPort.println(TITRE_TAB[TITRE_DUREE]);
#endif

	if (Message.containsKey("VITESSE"))
	{
		const char *Message_VITESSE = Message["VITESSE"];
#ifdef SDCARD_DEBUG
		SerialPort.print("Message_VITESSE : ");
		SerialPort.println(Message_VITESSE);
#endif
		tmpValueString = String(Message_VITESSE);

#ifdef SDCARD_DEBUG
		SerialPort.print("tmpValueString : ");
		SerialPort.println(tmpValueString);
#endif

#ifdef SDCARD_DEBUG
		SerialPort.print("Json Recup - ");
#endif
	}
	else
	{
		tmpValueString = DEFAULT_TITRE_VITESSE;
		MajFileParams = true;
#ifdef SDCARD_DEBUG
		SerialPort.print("Defaut Recup - ");
#endif
	}
	TITRE_TAB[TITRE_VITESSE] = tmpValueString.substring(0, MAX_CAR_TITRE_VITESSE);
#ifdef SDCARD_DEBUG
	SerialPort.print("Index : ");
	SerialPort.print(TITRE_VITESSE);
	SerialPort.print(" - Max car : ");
	SerialPort.println(MAX_CAR_TITRE_VITESSE);
	SerialPort.print("TITRE_VITESSE : ");
	SerialPort.println(TITRE_TAB[TITRE_VITESSE]);
#endif

	if (Message.containsKey("CONNECT"))
	{
		const char *Message_CONNECT = Message["CONNECT"];
#ifdef SDCARD_DEBUG
		SerialPort.print("Message_CONNECT : ");
		SerialPort.println(Message_CONNECT);
#endif
		tmpValueString = String(Message_CONNECT);

#ifdef SDCARD_DEBUG
		SerialPort.print("tmpValueString : ");
		SerialPort.println(tmpValueString);
#endif

#ifdef SDCARD_DEBUG
		SerialPort.print("Json Recup - ");
#endif
	}
	else
	{
		tmpValueString = DEFAULT_TITRE_CONNECT;
		MajFileParams = true;
#ifdef SDCARD_DEBUG
		SerialPort.print("Defaut Recup - ");
#endif
	}
	TITRE_TAB[TITRE_VITESSE] = tmpValueString.substring(0, MAX_CAR_TITRE_CONNECT);
#ifdef SDCARD_DEBUG
	SerialPort.print("Index : ");
	SerialPort.print(TITRE_CONNECT);
	SerialPort.print(" - Max car : ");
	SerialPort.println(MAX_CAR_TITRE_CONNECT);
	SerialPort.print("TITRE_CONNECT : ");
	SerialPort.println(TITRE_TAB[TITRE_CONNECT]);
#endif

	if (Message.containsKey("CONNECTA"))
	{
		const char *Message_CONNECTA = Message["CONNECTA"];
#ifdef SDCARD_DEBUG
		SerialPort.print("Message_CONNECTA : ");
		SerialPort.println(Message_CONNECTA);
#endif
		tmpValueString = String(Message_CONNECTA);

#ifdef SDCARD_DEBUG
		SerialPort.print("tmpValueString : ");
		SerialPort.println(tmpValueString);
#endif

#ifdef SDCARD_DEBUG
		SerialPort.print("Json Recup - ");
#endif
	}
	else
	{
		tmpValueString = DEFAULT_TITRE_CONNECTA;
		MajFileParams = true;
#ifdef SDCARD_DEBUG
		SerialPort.print("Defaut Recup - ");
#endif
	}
	TITRE_TAB[TITRE_CONNECTA] = tmpValueString.substring(0, MAX_CAR_TITRE_CONNECTA);
#ifdef SDCARD_DEBUG
	SerialPort.print("Index : ");
	SerialPort.print(TITRE_CONNECTA);
	SerialPort.print(" - Max car : ");
	SerialPort.println(MAX_CAR_TITRE_CONNECTA);
	SerialPort.print("TITRE_CONNECTA : ");
	SerialPort.println(TITRE_TAB[TITRE_CONNECTA]);
#endif

	if (Message.containsKey("DEMAR"))
	{
		const char *Message_DEMAR = Message["DEMAR"];
#ifdef SDCARD_DEBUG
		SerialPort.print("Message_DEMAR : ");
		SerialPort.println(Message_DEMAR);
#endif
		tmpValueString = String(Message_DEMAR);

#ifdef SDCARD_DEBUG
		SerialPort.print("tmpValueString : ");
		SerialPort.println(tmpValueString);
#endif

#ifdef SDCARD_DEBUG
		SerialPort.print("Json Recup - ");
#endif
	}
	else
	{
		tmpValueString = DEFAULT_TITRE_DEMAR;
		MajFileParams = true;
#ifdef SDCARD_DEBUG
		SerialPort.print("Defaut Recup - ");
#endif
	}
	TITRE_TAB[TITRE_DEMAR] = tmpValueString.substring(0, MAX_CAR_TITRE_DEMAR);
#ifdef SDCARD_DEBUG
	SerialPort.print("Index : ");
	SerialPort.print(TITRE_DEMAR);
	SerialPort.print(" - Max car : ");
	SerialPort.println(MAX_CAR_TITRE_DEMAR);
	SerialPort.print("TITRE_DEMAR : ");
	SerialPort.println(TITRE_TAB[TITRE_DEMAR]);
#endif

	if (Message.containsKey("REDEMAR"))
	{
		const char *Message_REDEMAR = Message["REDEMAR"];
#ifdef SDCARD_DEBUG
		SerialPort.print("Message_REDEMAR : ");
		SerialPort.println(Message_REDEMAR);
#endif
		tmpValueString = String(Message_REDEMAR);

#ifdef SDCARD_DEBUG
		SerialPort.print("tmpValueString : ");
		SerialPort.println(tmpValueString);
#endif

#ifdef SDCARD_DEBUG
		SerialPort.print("Json Recup - ");
#endif
	}
	else
	{
		tmpValueString = DEFAULT_TITRE_REDEMAR;
		MajFileParams = true;
#ifdef SDCARD_DEBUG
		SerialPort.print("Defaut Recup - ");
#endif
	}
	TITRE_TAB[TITRE_REDEMAR] = tmpValueString.substring(0, MAX_CAR_TITRE_REDEMAR);
#ifdef SDCARD_DEBUG
	SerialPort.print("Index : ");
	SerialPort.print(TITRE_REDEMAR);
	SerialPort.print(" - Max car : ");
	SerialPort.println(MAX_CAR_TITRE_REDEMAR);
	SerialPort.print("TITRE_REDEMAR : ");
	SerialPort.println(TITRE_TAB[TITRE_REDEMAR]);
#endif

	if (Message.containsKey("ENCOURS"))
	{
		const char *Message_ENCOURS = Message["ENCOURS"];
#ifdef SDCARD_DEBUG
		SerialPort.print("Message_ENCOURS : ");
		SerialPort.println(Message_ENCOURS);
#endif
		tmpValueString = String(Message_ENCOURS);

#ifdef SDCARD_DEBUG
		SerialPort.print("tmpValueString : ");
		SerialPort.println(tmpValueString);
#endif

#ifdef SDCARD_DEBUG
		SerialPort.print("Json Recup - ");
#endif
	}
	else
	{
		tmpValueString = DEFAULT_TITRE_ENCOURS;
		MajFileParams = true;
#ifdef SDCARD_DEBUG
		SerialPort.print("Defaut Recup - ");
#endif
	}
	TITRE_TAB[TITRE_ENCOURS] = tmpValueString.substring(0, MAX_CAR_TITRE_ENCOURS);
#ifdef SDCARD_DEBUG
	SerialPort.print("Index : ");
	SerialPort.print(TITRE_ENCOURS);
	SerialPort.print(" - Max car : ");
	SerialPort.println(MAX_CAR_TITRE_ENCOURS);
	SerialPort.print("TITRE_ENCOURS : ");
	SerialPort.println(TITRE_TAB[TITRE_ENCOURS]);
#endif

	if (Message.containsKey("CALIBR"))
	{
		const char *Message_CALIBR = Message["CALIBR"];
#ifdef SDCARD_DEBUG
		SerialPort.print("Message_CALIBR : ");
		SerialPort.println(Message_CALIBR);
#endif
		tmpValueString = String(Message_CALIBR);

#ifdef SDCARD_DEBUG
		SerialPort.print("tmpValueString : ");
		SerialPort.println(tmpValueString);
#endif

#ifdef SDCARD_DEBUG
		SerialPort.print("Json Recup - ");
#endif
	}
	else
	{
		tmpValueString = DEFAULT_TITRE_CALIBR;
		MajFileParams = true;
#ifdef SDCARD_DEBUG
		SerialPort.print("Defaut Recup - ");
#endif
	}
	TITRE_TAB[TITRE_CALIBR] = tmpValueString.substring(0, MAX_CAR_TITRE_CALIBR);
#ifdef SDCARD_DEBUG
	SerialPort.print("Index : ");
	SerialPort.print(TITRE_CALIBR);
	SerialPort.print(" - Max car : ");
	SerialPort.println(MAX_CAR_TITRE_CALIBR);
	SerialPort.print("TITRE_CALIBR : ");
	SerialPort.println(TITRE_TAB[TITRE_CALIBR]);
#endif

	if (Message.containsKey("VEILLE"))
	{
		const char *Message_VEILLE = Message["VEILLE"];
#ifdef SDCARD_DEBUG
		SerialPort.print("Message_VEILLE : ");
		SerialPort.println(Message_VEILLE);
#endif
		tmpValueString = String(Message_VEILLE);

#ifdef SDCARD_DEBUG
		SerialPort.print("tmpValueString : ");
		SerialPort.println(tmpValueString);
#endif

#ifdef SDCARD_DEBUG
		SerialPort.print("Json Recup - ");
#endif
	}
	else
	{
		tmpValueString = DEFAULT_TITRE_VEILLE;
		MajFileParams = true;
#ifdef SDCARD_DEBUG
		SerialPort.print("Defaut Recup - ");
#endif
	}
	TITRE_TAB[TITRE_VEILLE] = tmpValueString.substring(0, MAX_CAR_TITRE_VEILLE);
#ifdef SDCARD_DEBUG
	SerialPort.print("Index : ");
	SerialPort.print(TITRE_VEILLE);
	SerialPort.print(" - Max car : ");
	SerialPort.println(MAX_CAR_TITRE_VEILLE);
	SerialPort.print("TITRE_VEILLE : ");
	SerialPort.println(TITRE_TAB[TITRE_VEILLE]);
#endif

	// Close the file (Curiously, File's destructor doesn't close the file)
	file.close();

	//Mise à jour du fichier params.jso
	if (MajFileParams)
	{
#ifdef SDCARD_DEBUG
		SerialPort.println("Sauvegarde de nouveaux paramètres");
#endif
		//		saveConfigurationVario(filename);
	}
}

//**********************************************************
String VarioLanguage::getText(uint8_t value)
{
	//**********************************************************
	return TITRE_TAB[value];
}

/*

{
    "gnuvarioe": {
        "version": "1.0"
    },
		"language": {
				"id": 0,
				"text": "Français"
		},
    "titre": {
				"TIME":  "Heure",
				"TDV":   "Temps de vol",
				"ALTI":  "Alti",
				"AGL":   "Alti Sol",
				"VARIO": "Vario",
				"SPEED": "Vitesse",
				"FINESSE": "Finesse",
				"TCHUTE": "T Chute",
				"CAP": "Cap",
				"LAT": "Lat",
				"LONG": "Long",
				"COMPAS": "Compas"
    },
    "message": {
				"STAT": "Statistique",
				"DATE": "Date",
				"HEURE":"Heure",
				"DUREE": "Duree",
				"VITESSE": "Vitesse",
				"CONNECT": "Connection",
				"CONNECTA": "Connection a ",
				"DEMAR": "Demarrage",
				"REDEMAR": "Redemarrage",
				"ENCOURS": "en cours",		
				"CALIBR": "Calibration",
				"VEILLE": "En veille"
		}
}


JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(4) + 2*JSON_OBJECT_SIZE(12)+460
*/