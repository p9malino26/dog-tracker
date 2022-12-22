#include <SPI.h>
#include "MyLora.h"
#include <TinyGPSPlus.h>



//define the pins used by the transceiver module
#define ss 5
#define rst 14
#define dio0 2

#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  3        /* Time ESP32 will go to sleep (in seconds) */
#define nGpsRetries 5

RTC_DATA_ATTR uint8_t modem_config[] = {0x72, 0x74, 0x04};
RTC_DATA_ATTR LoRaClass LoRa(868E6, 0x21, ss, rst, dio0, modem_config);
RTC_DATA_ATTR TinyGPSPlus gps;
RTC_DATA_ATTR int lnl_time = -1;
RTC_DATA_ATTR double lat;
RTC_DATA_ATTR double longt;
RTC_DATA_ATTR bool has_been_loc = false;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial2.begin(9600);

  //setup LoRa transceiver module
  //LoRaClass LoRa(868E6, 0x21, ss, rst, dio0, modem_config);


  while (!LoRa.init()) {
    Serial.println(".");
    delay(500);
  }
   // Change sync word (0xF3) to match the receiver
  // The sync word assures you don't get LoRa messages from other LoRa transceivers
  // ranges from 0-0xFF
  Serial.println("LoRa Initializing OK!");
  /*while (true) {
    auto c= Serial2.read();
    Serial.print(c);
    if (gps.encode(c))
    {
        displayInfo();
        break;

    }
  }*/

  for (int i = 0; i < nGpsRetries; ++i) {
      while (true)  {
        if (Serial2.available())
          if (gps.encode(Serial2.read())) break;

      }
      if (gps.location.isValid()) break;
  }



  displayInfo();

  
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);

  delay(1000);
  Serial.flush(); 
  
  esp_deep_sleep_start();
  Serial.println("Should not call");


}

void displayInfo()
{
    
  LoRa.beginPacket();
  if (gps.location.isValid()){
    lat = gps.location.lat();
    longt = gps.location.lng();
    lnl_time = 0;
    has_been_loc = true;
    Serial.println("Location updated.");
  } else lnl_time = 1;

  if (has_been_loc) {
    LoRa.print(F("loc "));
    LoRa.print(lat, 6);
    LoRa.print(F(" "));
    LoRa.print(longt, 6);
    LoRa.print(F(" "));
    LoRa.print(lnl_time);
 
  } else {
    LoRa.print("nol");
    Serial.println("no location");

  }
  LoRa.sendPacket();
}

void displayInfo1()
{
    
  LoRa.beginPacket();
  LoRa.println("Loc info:");
  Serial.println("Loc info:");
  if (gps.location.isUpdated()) {
    LoRa.println("Updated");
    Serial.println("Updated");
  }
  if (gps.location.isValid()) {
    LoRa.println("Valid");
    Serial.println("Valid");
  } 

    double lat_ = gps.location.lat();
    double longt_ = gps.location.lng();

    LoRa.print(F("coords "));
    LoRa.print(lat_, 6);
    LoRa.print(F(" "));
    LoRa.println(longt_, 6);

    Serial.print(F("coords "));
    Serial.print(lat_, 6);
    Serial.print(F(" "));
    Serial.println(longt_, 6);

  
  LoRa.sendPacket();
}
/*
void loop() {
  // put your main code here, to run repeatedly:


  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    LoRa.beginPacket();
    LoRa.print(F("ERR"));
    LoRa.sendPacket();
    delay(3000);
  }
}*/

void loop() {

  }
