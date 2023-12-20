#include "Wire.h"
#include "SHT31.h"
#include "DHT.h"

#include <SPI.h>
#include <LoRa.h>

#define DHTPIN 7      // What digital pin we're connected to select yours accordingly
#define DHTTYPE DHT11 // DHT 11
DHT dht(DHTPIN, DHTTYPE);

SHT31 sht;

//"0" is receiving mode, "1" is transmitt mode
bool mode_change = 0;

unsigned long shtMillis = 0; // millis for reading sht30
unsigned long recMillis = 0; // millis for changing to receiving mode

// millis variables for updating dht11 value
unsigned long startMillis_DHT = 0;
const long updateDHT_Interval = 500;

// millis variables for reseting Lora Module
unsigned long startMillis_resetLora = 0;
const long resetLora_interval = 15000;

bool resetLora_flag = 0;

int val = 0;
float h, t;
String inString = "";

void setup()
{
  Serial.begin(9600);

  dht.begin();

  // Init Lora Module
  if (!LoRa.begin(433E6))
  { // or 915E6, the MHz speed of yout module
    Serial.println("Starting LoRa failed!");
    while (1)
      ;
  }

  // Initializing I2C Communication
  Wire.begin();
  sht.begin(0x44);
  Wire.setClock(100000);

  mode_change = 0;
  val = 0;

  Serial.println("Node 3 Start!!!");
}

void loop()
{

  unsigned long currentMillis = millis();

  if (currentMillis - startMillis_DHT >= updateDHT_Interval)
  {
    startMillis_DHT = currentMillis;
    h = dht.readHumidity();
    t = dht.readTemperature();
  }

  if (currentMillis - recMillis >= 200)
  {
    recMillis = currentMillis;

    int packetSize = LoRa.parsePacket();

    if (packetSize)
    {
      // read packet
      while (LoRa.available())
      {
        int inChar = LoRa.read();
        inString += (char)inChar;
        val = inString.toInt();
      }

      Serial.println(inString);

      if (inString == "1" || inString == "2")
      {
        inString = "";
        return;
      }
      if (inString == "3")
      {
        resetLora_flag = 1;
        if (LoRa.beginPacket())
        {
          Serial.println("Ready");
          LoRa.print(h);
          LoRa.print(",");
          LoRa.print(t);
          LoRa.endPacket();
        }
        inString = "";
      }
    }
  }

  // reset Lora after receive Data
  if (resetLora_flag == 1)
  {
    if (currentMillis - startMillis_resetLora >= resetLora_interval)
    {
      startMillis_resetLora = currentMillis;
      LoRa.end();
      if (!LoRa.begin(433E6))
      { 
        Serial.println("LoRa init failed. Check your connections.");
        while (true)
          ; // if failed, do nothing
      }
      Serial.print("Reset Lora DONE!");
      resetLora_flag = 0;
    }
  }
}
