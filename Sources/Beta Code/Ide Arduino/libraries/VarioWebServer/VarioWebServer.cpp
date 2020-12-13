#include "VarioWebServer.h"
#include "WebServer.h"
#include <Arduino.h>
#include <esp32-hal-log.h>
#include <libb64/cencode.h>
#include "WiFiServer.h"
#include "WiFiClient.h"
#include "FS.h"
#include "detail/RequestHandlersImpl.h"
#include "mbedtls/md5.h"

VarioWebServer::VarioWebServer(IPAddress addr, int port) : WebServer(addr, port)
{
}

VarioWebServer::VarioWebServer(int port) : WebServer(port)
{
}