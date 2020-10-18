/* VarioAlim -- 
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
 *                           VarioHardwareManager                                *
 *                                                                               *
 *  version    Date     Description                                              *
 *    1.0    27/07/20                                                            *
 *                                                                               *
 *********************************************************************************
 */

#ifndef VARIO_ALIM_H
#define VARIO_ALIM_H

class VarioAlim
{
public:
 //   VarioAlim();
    void init();
		
		long getVoltage(void);
		float getTension(void);
		int getCapacite(void);
		
		void enableAlim(void);
		void disableAlim(void);
};

#endif //VARIO_ALIM_H