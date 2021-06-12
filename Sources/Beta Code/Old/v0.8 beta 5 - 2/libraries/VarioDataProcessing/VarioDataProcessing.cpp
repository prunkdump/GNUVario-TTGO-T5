/* VarioDataProcessing -- 
 *
 * Copyright 2020 Jpg63
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
 *                           VarioData                                           *
 *                                                                               *
 *  version    Date     Description                                              *
 *    1.0    17/11/20                                                            *
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

#include "VarioDataProcessing.h"


//****************************************************************************************************************************
VarioDataProcessing::VarioDataProcessing()
//****************************************************************************************************************************
{
}

