#include <Arduino.h>
#include <VarioSqlFlightHelper.h>

VarioSqlFlightHelper::VarioSqlFlightHelper()
{
    varioSqlFlight = new VarioSqlFlight();
}

VarioSqlFlightHelper::~VarioSqlFlightHelper()
{
    varioSqlFlight->~VarioSqlFlight();
}

void VarioSqlFlightHelper::init(int16_t vlimit, int16_t voffset)
{
    limit = vlimit;
    offset = voffset;
    parcel = "";
    //ruse pour contourner le pb de mémoire sur sqlite
    limitTemp = limit;
    offsetTemp = offset;
    firstLine = true;
    isQueryInit = false;
    myRingBuffer.clear();
#ifdef SQL_DEBUG
    Serial.println("init");
#endif
}

void VarioSqlFlightHelper::init(String vParcel)
{
    parcel = vParcel;
    limit = 1000;
    offset = 0;

    //ruse pour contourner le pb de mémoire sur sqlite
    limitTemp = limit;
    offsetTemp = offset;
    firstLine = true;
    isQueryInit = false;
    myRingBuffer.clear();
#ifdef SQL_DEBUG
    Serial.println("init");
#endif
}

void VarioSqlFlightHelper::feedBuffer()
{
    bool res = false;

    if (limit > 0)
    {
        if (!isQueryInit)
        {
            if (limit > limitMax)
            {
                limitTemp = limitMax;
            }
            else
            {
                limitTemp = limit;
            }

            if (parcel == "")
            {
                if (!varioSqlFlight->initGetFlightsQuery(limitTemp, offsetTemp))
                {
                    //erreur requete
                    limit = 0;
                    return;
                }
            }
            else
            {
                if (!varioSqlFlight->initGetFlightsQuery(parcel))
                {
                    //erreur requete
                    limit = 0;
                    return;
                }
            }

            isQueryInit = true;

            if (firstLine)
            {
                myRingBuffer.push('[');
                //firstLine = false;
            }

            res = varioSqlFlight->getNextFlight(firstLine, myRingBuffer);
            if (res == false)
            {
                //pas la peine de continuer, on est a la fin
                limit = 0;
                myRingBuffer.push(']');
                return;
            }

            isQueryInit = true;
        }
        else
        {
            res = varioSqlFlight->getNextFlight(firstLine, myRingBuffer);
        }

        if (res == false)
        {
            isQueryInit = false;
            offsetTemp = offsetTemp + limitTemp;
            limit = limit - limitTemp;
            feedBuffer();
        }
    }
    else
    {
        myRingBuffer.push(']');
    }
}

uint16_t VarioSqlFlightHelper::readData(uint8_t *buffer, size_t maxLength)
{
    uint16_t bufLength;
    uint16_t nb = 0;

#ifdef SQL_DEBUG
    Serial.print("maxLength: ");
    Serial.println(maxLength);
#endif

    //si le buffer est vide, on tente de le remplir
    if (myRingBuffer.isEmpty())
    {
        if (limit <= 0)
        {
            return 0;
        }
        else
        {
#ifdef SQL_DEBUG
            Serial.println("avant feed");
#endif
            feedBuffer();
#ifdef SQL_DEBUG
            Serial.println("apres feed");
#endif
        }
    }

    //si le buffer est toujours vide, on renvoie 0 pour terminer
    if (myRingBuffer.isEmpty())
    {
        return 0;
    }

    size_t max = (ESP.getFreeHeap() / 3) & 0xFFE0;

    if (myRingBuffer.size() > maxLength)
    {
        bufLength = maxLength;
    }
    else
    {
        bufLength = myRingBuffer.size();
    }
    if (bufLength > max)
    {
        bufLength = max;
    }
// if (bufLength > 100)
// {
//     bufLength = 100;
// }
#ifdef SQL_DEBUG
    Serial.print("Buffer length: ");
    Serial.println(bufLength);
#endif

    char tBuf[bufLength];

    // si le buffer contient des données, on en envoie jusqu'a maxLength
    while (nb < bufLength && (!myRingBuffer.isEmpty()))
    {
        myRingBuffer.pop(tBuf[nb]);
        // Serial.print(tBuf[nb]);
        nb++;
    }

    if (nb > 0)
    {
        // tBuf[nb] = '\0';
        // nb++;
        // Serial.println("tBuf");
        // Serial.println(tBuf);
        memcpy(buffer, tBuf, nb);
    }

#ifdef SQL_DEBUG
    Serial.print("return nb ");
    Serial.println(nb);
#endif

    return nb;
}