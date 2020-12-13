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
 *    1.1    22/10/20   Ajout titre pour ecran batterie                          *
 *    1.1.1  24/10/20   Correction recup fichier de langue taille variable       *
 *                                                                               *
 *********************************************************************************
 */

#ifndef VARIO_LANGUAGE_H
#define VARIO_LANGUAGE_H

#include <HardwareConfig.h>

#define FRENCH 0
#define ENGLISH 1

#define TITRE_TIME 0
#define TITRE_TDV 1
#define TITRE_ALTI 2
#define TITRE_AGL 3
#define TITRE_VARIO 4
#define TITRE_SPEED 5
#define TITRE_FINESSE 6
#define TITRE_TCHUTE 7
#define TITRE_CAP 8
#define TITRE_LAT 9
#define TITRE_LONG 10
#define TITRE_COMPAS 11
#define TITRE_STAT 12
#define TITRE_DATE 13
#define TITRE_HEURE 14
#define TITRE_DUREE 15
#define TITRE_VITESSE 16
#define TITRE_CONNECT 17
#define TITRE_CONNECTA 18
#define TITRE_DEMAR 19
#define TITRE_REDEMAR 20
#define TITRE_ENCOURS 21
#define TITRE_CALIBR 22
#define TITRE_VEILLE 23
#define TITRE_CHARGE 24
#define TITRE_CHARGER 25
#define TITRE_BATTERIE 26

class VarioLanguage
{
public:
	void init(uint8_t language = 0);
	void loadConfigurationLangue(char *filename);
	String getText(uint8_t value);

private:
	uint8_t langue = 0;
	String TITRE_TAB[30];
};

extern VarioLanguage varioLanguage;

#endif //VARIO_DATA_H