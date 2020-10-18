/*
   esp32 firmware OTA
   Date: December 2018
   Author: Chris Joyce <https://chrisjoyce911/esp32FOTA>
   Purpose: Perform an OTA update from a bin located on a webserver (HTTP & HTTPS)
*/

/* 
 *********************************************************************************
 *                                                                               *
 *                           esp32fota2                                          *
 *                                                                               *
 *  version    Date     Description                                              *
 *    1.0    18/01/20                                                            *
 *    1.0.1  19/01/20   Ajout execHTTPexist et execHTTPSexist										 *
 *    1.0.2  23/01/20   Ajout String getHTTPVersion() et getHTTPSVersion()			 *
 *    1.0.3             Ajout downloadWwwFiles                                   *
 *    1.0.4  10/02/20   Ajout UpdateWwwDirectory                                 *
 *                                                                               *
 *********************************************************************************/

#ifndef esp32fota2_h
#define esp32fota2_h

#include "Arduino.h"

#define MAJ_ERROR 			-1
#define MAJ_AVAILABLE 		1
#define MAJ_NOTAVAILABLE	0

class esp32FOTA2
{
public:
  esp32FOTA2(String firwmareType, int firwmareVersion, int firwmareSubVersion, int firwmareBetaVersion);
  bool forceUpdate(String firwmareHost, int firwmarePort, String firwmarePath);
  void execOTA();
  bool execHTTPexist();
  bool execHTTPSexist();
  uint8_t execHTTPcheck(bool betaVersion = false);
  uint8_t execHTTPScheck(bool betaVersion = false);
  bool useDeviceID;
  String checkURL;
  int  UpdateVersion;
  int	 UpdateSubVersion;
	int  UpdateBetaVersion;	
	String getHTTPVersion();
	String getHTTPSVersion();
	bool UpdateWwwDirectory(void);

private:
  String getHeaderValue(String header, String headerName);
  String getDeviceID();
  String _firwmareType;
  int _firwmareVersion;
  int	_firwmareSubVersion;
	int _firwmareBetaVersion;
  String _host;
  String _bin;
  String _wwwfiles[10];
  uint8_t NB_WWW_FILES = 0;
  int _port;
  void downloadWwwFiles();
};

extern const char* rootCACertificate;

extern esp32FOTA2 esp32FOTA;
#endif