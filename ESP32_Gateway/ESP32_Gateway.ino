/*************************************************************
  Download latest Blynk library here:
    https://github.com/blynkkk/blynk-library/releases/latest

  Blynk is a platform with iOS and Android apps to control
  Arduino, Raspberry Pi and the likes over the Internet.
  You can easily build graphic interfaces for all your
  projects by simply dragging and dropping widgets.

    Downloads, docs, tutorials: http://www.blynk.cc
    Sketch generator:           http://examples.blynk.cc
    Blynk community:            http://community.blynk.cc
    Follow us:                  http://www.fb.com/blynkapp
                                http://twitter.com/blynk_app

  Blynk library is licensed under MIT license
  This example code is in public domain.

 *************************************************************

  This example shows how value can be pushed from Arduino to
  the Blynk App.

  WARNING :
  For this example you'll need Adafruit DHT sensor libraries:
    https://github.com/adafruit/Adafruit_Sensor
    https://github.com/adafruit/DHT-sensor-library

  App project setup:
    Value Display widget attached to V5
    Value Display widget attached to V6
 *************************************************************/

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPL6R2Y1E0F0"
#define BLYNK_TEMPLATE_NAME "Lora ESP32 Gateway"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <LoRa.h>
#include <SPI.h>

#define ss 4
#define rst 5
#define dio0 2

// pump pins
#define pump1 13
#define pump2 14
#define pump3 15

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "ACvZJLVxEC0QjjCTk39ONmAkj1v6MRlG"; // Paste auth token you copied

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "HshopLTK";      /// Enter your wifi name
char pass[] = "HshopLTK@2311"; // Enter wifi password

BlynkTimer timer;

bool check = 0;

// for flipping a bool every 1s
unsigned long startMillis = 0;
const unsigned long startMillis_interval = 1000;
bool temp = 0;

// for sending packet every 50ms
unsigned long startMillis1 = 0;
const unsigned long startMillis_interval1 = 50;

// each ID every 10s
unsigned long startMillis2 = 0;
const long modeChange_Interval = 10000;

const long send_Interval = 500;

int node_call = 3;

int test_node_change;

float tempx = 0;
float humix = 0;

// This function sends Arduino's up time every second to Virtual Pin (5).
// In the app, Widget's reading frequency should be set to PUSH. This means
// that you define how often to send data to Blynk App.
void sendSensor()
{

  Blynk.virtualWrite(V5, humix); // select your virtual pins accordingly

  Blynk.virtualWrite(V6, tempx); // select your virtual pins accordingly
}

void sendSensor1()
{
  Blynk.virtualWrite(V2, 5000); // select your virtual pins accordingly
}

void setup()
{

  delay(2000);
  // Debug console
  Serial.begin(115200);

  LoRa.setPins(ss, rst, dio0); // setup LoRa transceiver module
  LoRa.setSyncWord(0xA5);
  while (!LoRa.begin(433E6)) // 433E6 - Asia, 866E6 - Europe, 915E6 - North America
  {
    Serial.println(".");
    delay(500);
  }
  Serial.println("LoRa Initializing OK!");

  // pump motor Init
  pinMode(13, OUTPUT);
  pinMode(14, OUTPUT);
  pinMode(15, OUTPUT);

  digitalWrite(13, LOW);
  digitalWrite(14, LOW);
  digitalWrite(15, LOW);

  // Blynk Section
  Blynk.begin(auth, ssid, pass);
  timer.setInterval(1000L, sendSensor);
  timer.setInterval(1000L, sendSensor1);

  test_node_change = 1;
}

void loop()
{

  Blynk.run();
  timer.run();
  // First, a bool flip every 1s
  unsigned long currentMillis = millis();

  if (currentMillis - startMillis >= startMillis_interval)
  {
    startMillis = currentMillis;
    temp = !temp;
    Serial.println(test_node_change);
  }

  if (currentMillis - startMillis2 >= modeChange_Interval)
  {
    startMillis2 = currentMillis;
    test_node_change++;
    if (test_node_change > 3)
    {
      test_node_change = 1;
    }
  }

  switch (temp)
  {
  case 0:

    // send packet every 50ms
    if (currentMillis - startMillis1 >= startMillis_interval1)
    {
      startMillis1 = currentMillis;
      LoRa.beginPacket();
      LoRa.print(String(test_node_change));
      LoRa.endPacket();
    }
    break;

  case 1:
    int packetSize = LoRa.parsePacket();
    if (packetSize)
    {
      while (LoRa.available()) // read packet
      {
        String LoRaData = LoRa.readString();

        String temp1 = LoRaData.substring(0, 4);
        String humi1 = LoRaData.substring(4);
        tempx = temp1.toFloat();
        humix = humi1.toFloat();

        // Serial.print(tempx + "\t");
        // Serial.println(humix);

        Serial.println("Lora:" + LoRaData);
        Serial.println("Temp:" + temp1);
        Serial.println("Humid:" + humi1);
      }
    }
    break;
  }

  // // Một cái timer cứ mỗi 10s là cộng biến đếm thêm 1 ---> DONE!
  // if (currentMillis - startMillis1 >= modeChange_Interval)
  // {
  //   startMillis1 = currentMillis;
  //   test_node_change++;
  //   if (test_node_change > 3)
  //   {
  //     test_node_change = 1;
  //   }
  //   Serial.println(test_node_change);
  // }

  // /*Cách 2: Sử dụng Millis, cứ đúng thời điểm thì thành module gửi, còn lại trở thành module nhận */
  // if (currentMillis - startMillis >= send_Interval)
  // {
  //   startMillis = currentMillis;
  //   LoRa.beginPacket();
  //   LoRa.print(test_node_change);
  //   LoRa.endPacket();
  // }

  // int packetSize = LoRa.parsePacket(); // try to parse packet
  // if (packetSize)
  // {
  //   while (LoRa.available()) // read packet
  //   {
  //     String LoRaData = LoRa.readString();
  //     String temp1 = LoRaData.substring(0, 4);
  //     String humi1 = LoRaData.substring(6);

  //     tempx = temp1.toInt();
  //     humix = humi1.toInt();

  //     Serial.print(temp1 + "\t");
  //     Serial.println(humi1);
  //   }
  // }
}
