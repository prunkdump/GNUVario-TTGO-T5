/* WifiServer -- 
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

/* 
 *********************************************************************************
 *********************************************************************************
 *                                                                               *
 *                           WifiServer                                          *
 *                                                                               *
 *  version    Date     Description                                              *
 *    1.0    24/09/19                                                            *
 *    1.0.1  28/09/19   Changement de nom des librairies                         *
 *    1.1    06/10/19   Ajout serveur Web sur carte SD                           *
 *           11/12/19   Changement de nom                                        *
 *                                                                               *
 *********************************************************************************/
 
 /*
 *********************************************************************************
 *                    conversion image to cpp code                               *
 *                                                                               *
 *      https://javl.github.io/image2cpp/                                        *
 *                                                                               *
 *********************************************************************************/

#ifndef VARIOWIFISERVER_H
#define VARIOWIFISERVER_H

#include <HardwareConfig.h>

//#define ESP32WEBSERVEUR 
//#define ESPASYNCWEBSERVER

// Serveur sur carte SD
#include <VarioWifiServer_SD.h>

// Serveur interne
#include <VarioWifiServer_Int.h>

#endif