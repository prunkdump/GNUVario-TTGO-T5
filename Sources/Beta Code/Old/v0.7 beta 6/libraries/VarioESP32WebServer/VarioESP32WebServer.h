/* VarioESP32WebServer -- 
 *
 * Copyright 2019 Pascal - jpg63
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
 *                           VarioESP32WebServer                                 *
 *                                                                               *
 *  version    Date     Description                                              *
 *    1.0    27/12/19                                                            *
 *                                                                               *
 *********************************************************************************/
 
#ifndef VARIOESP32WEBSERVER_H
#define VARIOESP32WEBSERVER_H

#include <HardwareConfig.h>

#include <ESP32WebServer.h>

#define FILE_MAX_NAME   50

class VarioESP32WebServer : public ESP32WebServer
{
#define STREAMFILE_BUFSIZE 4096

public:
    VarioESP32WebServer(IPAddress addr, int port = 80);
    VarioESP32WebServer(int port = 80);

    //virtual ~VarioWebServer();

/*    template <typename T>
    size_t streamFile(T &file, const String &contentType)
    {
#ifdef SDFAT_LIB
			char fname[FILE_MAX_NAME];
			file.getName(fname, FILE_MAX_NAME);
      _streamFileCore(file.fileSize(), fname, contentType);
#else
        _streamFileCore(file.size(), file.name(), contentType);
#endif

        uint8_t *buf = (uint8_t *)malloc(STREAMFILE_BUFSIZE);
        if (buf == NULL)
        {
            return 0;
        }
        size_t fsize = 0;
        while (file.available())
        {
            int got = file.read(buf, STREAMFILE_BUFSIZE);
            _currentClient.write(buf, got);
            fsize += got;
            yield();
        }

        return fsize;
    }
};*/


		template<typename T> 
		size_t streamFile(T &file, const String& contentType)
		{
#ifdef SDFAT_LIB
			char fname[FILE_MAX_NAME];
			file.getName(fname, FILE_MAX_NAME);
			setContentLength(file.fileSize());
			if (String(fname).endsWith(".gz") &&
#else
			setContentLength(file.size());
			if (String(file.name()).endsWith(".gz") &&
#endif
		  contentType != "application/x-gzip" &&
			contentType != "application/octet-stream"){
			sendHeader("Content-Encoding", "gzip");
		}
		send(200, contentType, "");

#define STREAMFILE_BUFSIZE 2*1460

		uint8_t *buf = (uint8_t*)malloc(STREAMFILE_BUFSIZE);

		if (buf==NULL) {
			return 0;
		}

		size_t fsize = 0;

		while(file.available()){
			int got = file.read(buf, STREAMFILE_BUFSIZE);
			_currentClient.write(buf, got);

			fsize += got;

			yield();
		}

		return fsize;
	}
};

#endif //VARIOESP32WEBSERVER