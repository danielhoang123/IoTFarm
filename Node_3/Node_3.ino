#include "Wire.h"
#include "SHT31.h"

#include <SPI.h>
#include <LoRa.h>

#include <Servo.h>

Servo myservo;

unsigned int temp1, temp2;

signed int temp3;

unsigned long startMillis = 0;
const long interval = 10;

unsigned long startMillis1 = 0;
const long interval1 = 1000;

SHT31 sht;

//"0" is receiving mode, "1" is transmitt mode
bool mode_change = 0;

unsigned long shtMillis = 0; // millis for reading sht30
unsigned long recMillis = 0; // millis for changing to receiving mode

// millis variables for updating dht11 value
unsigned long startMillis_DHT = 0;
const long updateDHT_Interval = 250;

// millis variables for reseting Lora Module
unsigned long startMillis_LORAsend = 0;
const long LORAsend_interval = 50;

// reset bool temp
unsigned long startMillis_clearString = 0;
const long clearString_interval = 500;

// millis for controlling servo
unsigned long startMillis_servoControl = 0;
const long servoControl_interval = 1000;

bool resetLora_flag = 0;

int val = 0, count = 0;
float h, t;

bool temp = 0;

String inString = "";

void setup()
{
  Serial.begin(9600);
  myservo.attach(5);
  myservo.write(90);
  // dht.begin();

  // Init Lora Module
  // if (!LoRa.begin(433E6))
  // { // or 915E6, the MHz speed of yout module
  //   Serial.println("Starting LoRa failed!");
  //   while (1)
  //     ;
  // }

  // LoRa.setPins(10, 9, 2); // setup LoRa transceiver module
  // LoRa.setSyncWord(0xA5);
  while (!LoRa.begin(433E6)) // 433E6 - Asia, 866E6 - Europe, 915E6 - North America
  {
    Serial.println(".");
    delay(500);
  }
  Serial.println("LoRa Initializing OK!");

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

  if (currentMillis - startMillis_DHT >= 50)
  {
    startMillis_DHT = currentMillis;
    sht.read();
    t = sht.getTemperature();
    h = sht.getHumidity();
    // Serial.println("Temp: " + String(t) + ", Humid: " + String(h));
  }

  if (currentMillis - recMillis >= 100)
  {
    recMillis = currentMillis;
    int packetSize = LoRa.parsePacket();
    if (packetSize)
    {
      while (LoRa.available())
      {
        int inChar = LoRa.read();
        inString += (char)inChar;
        val = inString.toInt();
      }
      Serial.println(inString);

      if (inString == "3")
      {
        temp = 1;
      }

      inString = "";
    }
  }

  if (temp == 1)
  {
    // Serial.println("Checked");
    LoRa.beginPacket();
    LoRa.print(t, 1);
    LoRa.print(",");
    LoRa.print(h, 1);
    LoRa.endPacket();

    if (currentMillis - startMillis_clearString >= clearString_interval)
    {
      startMillis_clearString = currentMillis;
      temp = 0;
    }
  }

  // switch (temp)
  // {
  // case 0:
  //   int packetSize = LoRa.parsePacket();

  //   if (packetSize)
  //   {
  //     while (LoRa.available())
  //     {
  //       int inChar = LoRa.read();
  //       inString += (char)inChar;
  //       val = inString.toInt();
  //     }
  //     Serial.println(inString);

  //     if (inString == "Hello")
  //     {
  //       temp = 1;
  //       Serial.println(temp);
  //       inString = "";
  //     }
  //     else
  //     {
  //       inString = "";
  //     }
  //   }

  //   break;
  // case 1:
  //   unsigned long currentMillis1 = millis();
  //   if (currentMillis1 - startMillis_LORAsend >= 10000)
  //   {
  //     startMillis_LORAsend = currentMillis1;
  //     temp = 0;
  //   }

  //   if (count == 8)
  //   {
  //     temp = 0;
  //     count = 0;
  //     Serial.println("temp: " + String(temp) + "and Count: " + String(count));
  //   }

  //   LoRa.beginPacket();
  //   LoRa.print(t, 1);
  //   LoRa.print(h, 1);
  //   LoRa.endPacket();

  //   break;
  // }

  // reset Lora after receive Data
  // if (resetLora_flag == 1)
  // {
  //   if (currentMillis - startMillis_resetLora >= resetLora_interval)
  //   {
  //     startMillis_resetLora = currentMillis;
  //     LoRa.end();
  //     if (!LoRa.begin(433E6))
  //     {
  //       Serial.println("LoRa init failed. Check your connections.");
  //       while (true)
  //         ; // if failed, do nothing
  //     }
  //     Serial.print("Reset Lora DONE!");
  //     resetLora_flag = 0;
  //   }
  // }

  /*START Servo Control*/
  temp1 = analogRead(A1);
  temp2 = analogRead(A2);

  if (currentMillis - startMillis >= 50)
  {
    startMillis = currentMillis;
    temp3 = temp2 - temp1;
  }

  if (currentMillis - startMillis1 >= interval1)
  {
    startMillis1 = currentMillis;
    if (temp3 <= -50)
    {
      myservo.write(135);
    }

    if (temp3 > -50 && temp3 <= -20)
    {
      myservo.write(115);
    }

    if (temp3 > -10 && temp3 <= 10)
    {
      myservo.write(90);
    }

    if (temp3 > 20 && temp3 <= 50)
    {
      myservo.write(85);
    }

    if (temp3 >= 50)
    {
      myservo.write(45);
    }
  }
  /*END Servo Control*/
}
