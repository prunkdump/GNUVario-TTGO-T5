/* varioscreenGxEPD -- 
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
 *                           VarioScreenGxEPD                                    *
 *                                                                               *
 *  version    Date     Description                                              *
 *    1.0    24/09/19                                                            *
 *    1.0.1  28/09/19   Changement de nom des librairies                         *
 *    1.0.2  22/10/19   Ajout écran 2.13''                                       *
 *    1.0.3  03/02/20   Changement nom passage à 290 au lieu de 29               *
 *    1.0.4  10/02/20   Compatibilité écran 291 et 293                           *
 *                      Compatibilité écran 290 et 292                           *
 *                                                                               *
 *********************************************************************************/
 
 /*
 *********************************************************************************
 *                    conversion image to cpp code                               *
 *                                                                               *
 *      https://javl.github.io/image2cpp/                                        *
 *                                                                               *
 *********************************************************************************/

#ifndef VARIOSCREENGXEPD_H
#define VARIOSCREENGXEPD_H

#include <HardwareConfig.h>

// Ecran 1.54''
#if (VARIOSCREEN_SIZE == 154)
#include <varioscreenObjects_154.h>
#include <varioscreenGxEPD_154.h>
#endif

// Ecran 2.90'' paysage
#if (VARIOSCREEN_SIZE == 290)
#include <varioscreenGxEPD_290b.h>
#include <varioscreenObjects_290b.h>
#endif

// Ecran 2.90'' V2 paysage
#if (VARIOSCREEN_SIZE == 292)
#include <varioscreenGxEPD_290b.h>
#include <varioscreenObjects_290b.h>
#endif

// Ecran 2.90'' portrait
#if (VARIOSCREEN_SIZE ==291)
#include <varioscreenGxEPD_291b.h>
#include <varioscreenObjects_291b.h>
#endif

// Ecran 2.90''V2 portrait
#if (VARIOSCREEN_SIZE ==293)
#include <varioscreenGxEPD_291b.h>
#include <varioscreenObjects_291b.h>
#endif


// Ecran 2.13''
//#include <varioscreenGxEPD_213.h>
//#include <varioscreenObjects_213.h>

#endif