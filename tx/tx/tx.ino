#include <SPI.h>
#include "MyLora.h"
#include <TinyGPSPlus.h>

TinyGPSPlus gps;

//define the pins used by the transceiver module
#define ss 5
#define rst 14
#define dio0 2

uint8_t modem_config[] = {0x72, 0x74, 0x04};
LoRaClass LoRa(868E6, 0x21, ss, rst, dio0, modem_config);
int lnl_time = -1;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial2.begin(9600);

  //setup LoRa transceiver module

  while (!LoRa.init()) {
    Serial.println(".");
    delay(500);
  }
   // Change sync word (0xF3) to match the receiver
  // The sync word assures you don't get LoRa messages from other LoRa transceivers
  // ranges from 0-0xFF
  Serial.println("LoRa Initializing OK!");

}

void displayInfo()
{
    
  LoRa.beginPacket();
  if (gps.location.isUpdated()){
    LoRa.print(F("LOC "));
    LoRa.print(gps.location.lat(), 6);
    LoRa.print(F(" "));
    LoRa.print(gps.location.lng(), 6);
    lnl_time = millis() / 1000;
    Serial.println("Location valid");
  }
  else if (lnl_time != -1)
  {
    LoRa.print(F("LNL "));
    LoRa.print(gps.location.lat(), 6);
    LoRa.print(F(" "));
    LoRa.print(gps.location.lng(), 6);
    LoRa.print(F(" "));
    LoRa.print(millis() / 1000 - lnl_time);
    Serial.println("lnl");

  } else {
    LoRa.print("NOL");
    Serial.println("no location");

  }
  LoRa.sendPacket();
}

void loop() {
  // put your main code here, to run repeatedly:
  while (Serial2.available() > 0) {
    if (gps.encode(Serial2.read()))
    {
        displayInfo();

        delay(10000);

    }
  }

  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    LoRa.beginPacket();
    LoRa.print(F("ERR"));
    LoRa.sendPacket();
    delay(3000);
  }
}
