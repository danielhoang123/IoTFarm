#include "Wire.h"
#include "SHT31.h"

#include <SPI.h>
#include <LoRa.h>

#include <Servo.h>

#define tolerance 18

Servo myservo;

unsigned int temp1, temp2;

signed int temp3;

unsigned long startMillis = 0;
const long interval = 10;

unsigned long startMillis1 = 0;
const long interval1 = 50;
int servo_pos;

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

int count = 0;
float h, t;

bool temp = 0;

String inString = "";

void setup()
{
  Serial.begin(9600);
  
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

  /*START Servo Setup*/
  delay(250);

  myservo.attach(5);
  myservo.write(110);

  delay(1000);

  temp1 = analogRead(A1);
  delayMicroseconds(250);
  temp2 = analogRead(A2);
  delayMicroseconds(250);
  temp3 = temp2 - temp1;
  Serial.println(temp3);

  // sun 180 degree (Sunrise)
  if (temp3 <= -900)
  {
    servo_pos = 175;
    myservo.write(servo_pos);
    delay(500);
  }

  // between Sunrise and Noon
  if (temp3 >= -900 && temp3 <= -20)
  {
    servo_pos = 150;
    myservo.write(servo_pos);
    delay(500);
  }

  // sun 90 degree (Noon)
  if (temp3 >= -20 && temp3 <= 20)
  {
    servo_pos = 105;
    myservo.write(servo_pos);
    delay(500);
  }

  // between Noon and Susnet
  if (temp3 >= 20 && temp3 <= 900)
  {
    servo_pos = 60;
    myservo.write(servo_pos);
    delay(500);
  }

  // sun 0 degree (Sunset)
  if (temp3 >= 900)
  {
    servo_pos = 40;
    myservo.write(servo_pos);
    delay(500);
  }

  /*END Servo Setup*/

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

  if (currentMillis - startMillis >= 50)
  {
    startMillis = currentMillis;
    temp1 = analogRead(A1);
    delayMicroseconds(250);
    temp2 = analogRead(A2);
    delayMicroseconds(250);
    temp3 = temp2 - temp1;
  }

  if (currentMillis - startMillis1 >= interval1)
  {
    startMillis1 = currentMillis;
    int tempx = temp3;
    if (tempx < -tolerance)
    {
      if (servo_pos < 175)
      {
        myservo.write(servo_pos);
        servo_pos++;
      }
    }
    if (tempx > tolerance)
    {
      if (servo_pos > 40)
      {
        servo_pos--;
        myservo.write(servo_pos);
      }
    }
  }
  /*END Servo Control*/
}
