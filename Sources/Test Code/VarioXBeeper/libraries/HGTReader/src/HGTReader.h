/* HGTReader -- 
 *
 * Copyright 2020 JeromeV
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
 *                           HGTReader                                           *
 *                                                                               *
 *  version    Date     Description                                              *
 *    1.0    07/03/20                                                            *
 *                                                                               *
 *********************************************************************************
 */

#ifndef HGT_READER_H
#define HGT_READER_H

#include <sdcardHAL.h>

#define FILE_EXTENSION ".HGT"
#define NO_FILE_FOR_POS -1
#define NB_PT_PER_ROW 1201
#define NB_POS_DEC 6
#define NB_SEC_PER_DEG 3600
#define NB_SEC_PAR_TILE 3

class HGTReader
{
public:
  HGTReader(const String &aglDir);
  int getGroundLevel(float latitude, float longitude);

private:
  String getFileNameForPosition(float latitude, float longitude);
  bool openFile(const String &fileName);
  int loadGroundLevel(float latitude, float longitude);
  int getTileHeight(int x, int y);
  static String rightPad(const String &src, int size, char c);

  String aglDir;
  String currentFileName = "";
  File currentFile;
};

#endif //HGT_READER_H