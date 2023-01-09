#include <SPI.h>
#include "MyLora.h"
#include "NMEAParser.h"



//define the pins used by the transceiver module
#define ss 5
#define rst 14
#define dio0 2

#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  10        /* Time ESP32 will go to sleep (in seconds) */
#define nGpsRetries 5

//good one 0x64, 0xa4, 0xc4
RTC_DATA_ATTR uint8_t modem_config[] = {0x72, 0x74, 0x04};
RTC_DATA_ATTR LoRaClass LoRa(868E6, 0x21, ss, rst, dio0, modem_config);
RTC_DATA_ATTR int lnl_time = -1;
RTC_DATA_ATTR double lat;
RTC_DATA_ATTR double longt;
RTC_DATA_ATTR bool has_been_loc = false;

NMEAParser gps;

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

  Serial2.flush();
  while (true) {
      while (!Serial2.available()); 
      if(gps.feed((char)Serial2.read())) {
          displayInfo();
          break;
      }
  }

  
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);

  delay(1000);
  Serial.flush(); 
  
  esp_deep_sleep_start();
  Serial.println("Should not call");


}

void displayInfo()
{
    
  LoRa.beginPacket();
  if (gps.isValid()){
    lat = gps.lat();
    longt = gps.lng();
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

void loop() {

  }
