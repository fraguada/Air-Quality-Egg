
/* sensor vars and read functions */
#include <EtherCard.h>
#include <SoftReset.h>
#include <AQERF_Base.h>
#include <avr/wdt.h>
#include <string.h>
#include "MemoryLocations.h"

extern char website[] PROGMEM;

long previousCosmResponseTimestamp = 0;
const long TOO_LONG_FOR_COSM_RESPONSE = 600000; // 10 minutes

Stash stash;
static byte tcp_session;

#define APIKEY "YOURAPIKEY" // put your key here
#define USERNAME "YOURUSERNAME"

void postSensorDataToAIO(){

  byte sd = stash.create();
  stash.print("{\"value\":\"");
  stash.print(rflink.getSensorValue());
  stash.print("\"}");
  stash.save();

  Serial.println(F("Preparing stash"));

  //TODO: Change feed according to sensor Type rflink.getSensorType()
  //TODO: store username in variable

  Serial.print("Sensor to send: ");
  Serial.println(rflink.getSensorType());

  if(strcmp(rflink.getSensorType(), "Dust") == 0)
  {

    Stash::prepare(PSTR("POST http://io.adafruit.com/api/v2/$F/feeds/$F/data HTTP/1.1" "\r\n"
      "Host: $F" "\r\n"
      "X-AIO-Key: $F" "\r\n"
      "Content-Type: application/json" "\r\n"
      "Content-Length: $D" "\r\n"
      "\r\n"
      "$H"),
    PSTR(USERNAME), PSTR("aqedust"),website, PSTR(APIKEY), stash.size(), sd);

    Serial.println(F("Sending data to Adafruit IO"));

    tcp_session = ether.tcpSend();
    Serial.println(F("Data sent"));
    return;
      //dispatch("aqedust", stash, sd);
  }

  if(strcmp(rflink.getSensorType(), "CO") == 0)
  {
    Stash::prepare(PSTR("POST http://io.adafruit.com/api/v2/$F/feeds/$F/data HTTP/1.1" "\r\n"
      "Host: $F" "\r\n"
      "X-AIO-Key: $F" "\r\n"
      "Content-Type: application/json" "\r\n"
      "Content-Length: $D" "\r\n"
      "\r\n"
      "$H"),
    PSTR(USERNAME), PSTR("aqeco"), website, PSTR(APIKEY), stash.size(), sd);

    Serial.println(F("Sending data to Adafruit IO"));

    tcp_session = ether.tcpSend();
    Serial.println(F("Data sent"));
    return;
  }

  if(strcmp(rflink.getSensorType(), "NO2") == 0)
  {
    Stash::prepare(PSTR("POST http://io.adafruit.com/api/v2/$F/feeds/$F/data HTTP/1.1" "\r\n"
      "Host: $F" "\r\n"
      "X-AIO-Key: $F" "\r\n"
      "Content-Type: application/json" "\r\n"
      "Content-Length: $D" "\r\n"
      "\r\n"
      "$H"),
    PSTR(USERNAME), PSTR("aqeno2"), website, PSTR(APIKEY), stash.size(), sd);

    Serial.println(F("Sending data to Adafruit IO"));

    tcp_session = ether.tcpSend();
    Serial.println(F("Data sent"));
    return;
  }

  if(strcmp(rflink.getSensorType(), "Temperature") == 0)
  {
    Stash::prepare(PSTR("POST http://io.adafruit.com/api/v2/$F/feeds/$F/data HTTP/1.1" "\r\n"
      "Host: $F" "\r\n"
      "X-AIO-Key: $F" "\r\n"
      "Content-Type: application/json" "\r\n"
      "Content-Length: $D" "\r\n"
      "\r\n"
      "$H"),
    PSTR(USERNAME), PSTR("aqetemp"), website, PSTR(APIKEY), stash.size(), sd);

    Serial.println(F("Sending data to Adafruit IO"));

    tcp_session = ether.tcpSend();
    Serial.println(F("Data sent"));
    return;
  }

  if(strcmp(rflink.getSensorType(), "Humidity") == 0)
  {
    Stash::prepare(PSTR("POST http://io.adafruit.com/api/v2/$F/feeds/$F/data HTTP/1.1" "\r\n"
      "Host: $F" "\r\n"
      "X-AIO-Key: $F" "\r\n"
      "Content-Type: application/json" "\r\n"
      "Content-Length: $D" "\r\n"
      "\r\n"
      "$H"),
    PSTR(USERNAME), PSTR("aqerh"), website, PSTR(APIKEY), stash.size(), sd);

    Serial.println(F("Sending data to Adafruit IO"));

    tcp_session = ether.tcpSend();
    Serial.println(F("Data sent"));
    return;
  }

}

void dispatch(char * feed, Stash sta, byte s){

  Stash::prepare(PSTR("POST http://io.adafruit.com/api/v2/$F/feeds/$F/data HTTP/1.1" "\r\n"
      "Host: $F" "\r\n"
      "X-AIO-Key: $F" "\r\n"
      "Content-Type: application/json" "\r\n"
      "Content-Length: $D" "\r\n"
      "\r\n"
      "$H"),
    PSTR(USERNAME), feed, website, PSTR(APIKEY), sta.size(), s);

    Serial.println(F("Sending data to Adafruit IO"));

    tcp_session = ether.tcpSend();
    Serial.println(F("Data sent"));

}

void addressToString(uint8_t * address, char * target, char delimiter){
  uint8_t target_index = 0;
  for(uint8_t jj = 0; jj < 6; jj++){
    snprintf_P(target + target_index, 3, PSTR("%02X"), address[jj]);
    target_index+=2;
    if(jj != 5 ) target[target_index++] = delimiter;
  }
}

//TODO: Change name
void checkCosmReply(){
  const char *reply = ether.tcpReply(tcp_session);
  if(reply != 0){
    Serial.println(F(">>> RESPONSE RECEIVED ---"));
    Serial.println(reply);
    markCosmResponse();

  }

}

void stringConvertMAC(uint8_t * mac, char * buf, char delimiter){
  for(uint8_t ii = 0; ii < 6; ii++){
    convertByteArrayToAsciiHex(mac + ii, buf + 3*ii, 1);
    if(ii == 5) buf[3*ii+2] = '\0';
    else buf[3*ii+2] = delimiter;
  }
}

//TODO: Change Name
boolean haventHeardFromCosmLately(){
  unsigned long currentMillis = millis();
  if((currentMillis - previousCosmResponseTimestamp) > TOO_LONG_FOR_COSM_RESPONSE){
    return true;
  }
  return false;
}

//TODO: Change Name
void markCosmResponse(){
  previousCosmResponseTimestamp = millis();

}
