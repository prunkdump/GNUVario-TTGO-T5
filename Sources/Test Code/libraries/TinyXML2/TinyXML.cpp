/* LCD library for noka 3110 display
 * Date: January 2010
 * Author: J Crouchley
 *
 * This is a table driven parser for simple XML,
 * Pass the XML into the library one character at a time
 * the callback function will be called at 'interesting'
 * point (got a tag, got an attribute value, tag text, end of tag)
 * with information about the tags and any values.
 *
 * See the examples for usage
 */
#include <Arduino.h>
#include <inttypes.h>

#include "TinyXMLTable.hpp"

#include "TinyXML.h"


#define DEBUG 2     // debug level 4

parseTable *pTable = (parseTable *)&stateTable;


TinyXML::TinyXML()
{
}

void TinyXML::init(uint8_t* buffer, uint16_t maxbuflen, XMLcallback XMLcb)
{
  Xcb = XMLcb;
  dataBuffer = buffer;
  maxDataLen = maxbuflen;
  reset();
}

void TinyXML::reset()
{
  dataBufferPtr =0;
  tagBufferPtr = 0;
  LTCount = 0;
  tagCount = 0;
  currentState = Init;
}

void TinyXML::processChar(uint8_t ch)
{
  uint16_t chToParse;
  boolean bMatch=false;
  while (!bMatch)
  {
    chToParse = pgm_read_word(&(pTable[currentState].charToParse));
    switch ( chToParse )
    {
    case whiteSpace:
      if (ch == ' ' || ch == '\t' || ch == '\n' | ch == '\r') bMatch=true;
      break;
    case alpha:
      if (isAlpha(ch))  bMatch=true;
      break;
    case alphanum:
      if (isAlpha(ch) || isNumeric(ch) || (ch == ':') || (ch == '_') || (ch == '-'))  bMatch=true;
      break;
    case quote:
      if (ch == '"' || ch == '\'')
      {
        matchQuote = ch;
        bMatch=true;
      }
      break;
    case matchingquote:
      if (ch == matchQuote) bMatch=true;
      break;
    case anychar:
      bMatch=true;
      break;
    default:
      if (ch == chToParse) bMatch=true;
      break;
    }
    if (!bMatch)
    {
#if DEBUG > 3
      Serial.print("Non-matching state:");
      Serial.print(currentState,DEC);
      Serial.print(" ch:");
      Serial.print(ch,HEX);
      Serial.print(" match criteria:");
      Serial.print(chToParse,HEX);
      Serial.print(" new state:");
      Serial.println(currentState+1,DEC);
#endif
      currentState++;
    }
  } // as every table enry must end in anychar we must get out of here

#if DEBUG > 2
  Serial.print("Matching state:");
  Serial.print(currentState,DEC);
  Serial.print(" ch:");
  Serial.print(ch,HEX);
  Serial.print(" match criteria:");
  Serial.print(chToParse,HEX);
    Serial.print(" tagBufferPtr:");
    Serial.print(tagBufferPtr,DEC);
  Serial.print(" new state:");
  Serial.println(pgm_read_byte(&(pTable[currentState].nextState)),DEC);
#endif
  action(ch, pgm_read_byte(&(pTable[currentState].actionNumber)));
  action(ch, pgm_read_byte(&(pTable[currentState].actionNumber2)));

  currentState=pgm_read_byte(&(pTable[currentState].nextState));
}



void TinyXML::action(uint8_t ch, uint8_t actionType)
{
#if DEBUG > 5
  Serial.print("Action:");
  Serial.println(actionType,DEC);
#endif
  switch (actionType)
  {
  case donothing:
    break;
  case incLTcount:
    LTCount++;
    break;
  case decLTcount:
    if (--LTCount < 0 ) action(ch,error);
    break;
  case cleardata:
    dataBufferPtr = 0;
    break;
  case storeifneeded:
    if (dataBufferPtr < maxDataLen-2) dataBuffer[dataBufferPtr++] = ch;
    break;
  case starttagname:
    dataBuffer[dataBufferPtr] = 0; // terminate the text
    // call back if the previous tag text is required
    tagBuffer[tagBufferPtr] = 0;
    if (tagBufferPtr && dataBufferPtr) Xcb(STATUS_TAG_TEXT,(char*)tagBuffer,tagBufferPtr,(char*)dataBuffer,dataBufferPtr);
    dataBufferPtr = 0;    // clear down for next time
#if DEBUG > 2
    Serial.print("starttagname Tag:");
    Serial.print((char*)tagBuffer);
    Serial.print(" text:");
    Serial.println((char*)dataBuffer);
#endif
    if (tagBufferPtr < TAGBUFFERMAX-2) tagBuffer[tagBufferPtr++] = '/';
    else action(ch, error);
    break;
  case cleartagname:
    tagBuffer[--tagBufferPtr]=0;  // remove the slash
    break;
  case addtotagname:
    if (tagBufferPtr < TAGBUFFERMAX-2) tagBuffer[tagBufferPtr++] = ch;
    else action(ch, error);
    break;
  case inctagcount:
    tagCount++;
    tagBuffer[tagBufferPtr] = 0;
    Xcb(STATUS_START_TAG,(char*)tagBuffer,tagBufferPtr,0,0);
#if DEBUG > 2
    Serial.print("incTagCount:");
    Serial.print(tagCount,DEC);
    Serial.print(" Tags found:");
    Serial.println((char*)tagBuffer);
#endif
    break;
  case removelasttag:
    if (--tagCount < 0 )
	{
		action(ch,error);
		break;
	}
    tagBuffer[tagBufferPtr] = 0;
    Xcb(STATUS_END_TAG,(char*)tagBuffer,tagBufferPtr,0,0);
    while (tagBufferPtr && tagBuffer[--tagBufferPtr] != '/'); // as we error if tagBuffer overflows then this will be safe
#if DEBUG > 3
    tagBuffer[tagBufferPtr] = 0;
    Serial.print("removelasttag TagCount:");
    Serial.print(tagCount,DEC);
    Serial.print(" tagBufferPtr:");
    Serial.print(tagBufferPtr,DEC);
    Serial.print(" Tags found:");
    Serial.println((char*)tagBuffer);
#endif
    break;
  case cleartagendname:
    checkTagBufferPtr = 0;
    break;
  case addtochktagname:
    if (tagBufferPtr < CHECKTAGMAX-2) checkTagBuffer[checkTagBufferPtr++] = ch;
    else action(ch, error);
    break;
  case checkremovelasttag:
    // need to test here to see if the tag being removed is the correct one - error if not
    if (--tagCount < 0 )
	{
		action(ch,error);
		break;
	}
    tagBufferPtr--;      // we have had a start so back past the last '/' we placed when the tag started
    tagBuffer[tagBufferPtr] = 0;
    Xcb(STATUS_END_TAG,(char*)tagBuffer,tagBufferPtr,0,0);
    while (tagBufferPtr && tagBuffer[--tagBufferPtr] != '/'); // as we error if tagBuffer overflows then this will be safe
#if DEBUG > 3
    tagBuffer[tagBufferPtr] = 0;
    Serial.print("checkremovelasttag TagCount:");
    Serial.print(tagCount,DEC);
    Serial.print(" tagBufferPtr:");
    Serial.print(tagBufferPtr,DEC);
    Serial.print(" Tags found:");
    Serial.println((char*)tagBuffer);
#endif
    break;
  case clearattrname:
    attrBufferPtr = 0;
    dataBufferPtr = 0;
    break;
  case addtoattrname:
    attrBuffer[attrBufferPtr++] = ch;
    break;
  case setquotechar:
    matchQuote = ch;
    break;
  case addtoattrvalue:
    if (dataBufferPtr < maxDataLen-2) dataBuffer[dataBufferPtr++] = ch;
    break;
  case gotattrvalue:
    attrBuffer[attrBufferPtr] = 0;
    dataBuffer[dataBufferPtr] = 0;
    Xcb(STATUS_ATTR_TEXT,(char*)attrBuffer,attrBufferPtr,(char*)dataBuffer,dataBufferPtr);
    break;
  case error:
    Xcb(STATUS_ERROR,(char*)tagBuffer,tagBufferPtr,(char*)dataBuffer,dataBufferPtr);
    reset();
   break;
  case initialise:
    reset();
    break;
  }
}


