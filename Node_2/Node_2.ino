/*
  Example of BH1750 library usage. This example initialises the BH1750 object using the default high resolution continuous mode and then makes a light level reading every second.
*/
#include <SoftwareSerial.h>
#include <Wire.h>
#include <BH1750.h>
#include <SPI.h>
#include <LoRa.h>
#include <Servo.h>

#define tolerance 18
Servo myservo;
BH1750 lightMeter;

/*START Variables to control servo*/
unsigned int temp1, temp2;

signed int temp3;
signed int check1 = -900;

unsigned long startMillis = 0;
const long interval = 10;

unsigned long startMillis1 = 0;
const long interval1 = 1000;

int servo_pos;
/*END Variables to control servo*/

const byte ph[] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x01, 0x84, 0x0A};
byte values[11];
// SoftwareSerial mod(2, 3);

float lux = 0.0;

float soil_ph = 0;

unsigned long bhMillis = 0;
unsigned long phMillis = 0;

unsigned long recMillis = 0; // millis for changing to receiving mode
String inString = "";
int val = 0;
bool temp = 0;

unsigned long startMillis_clearString = 0;
const long clearString_interval = 500;

void servo_first_time();

void setup()
{
    Serial.begin(4800);

    while (!LoRa.begin(433E6)) // 433E6 - Asia, 866E6 - Europe, 915E6 - North America
    {
        delay(500);
    }

    // Initialize the I2C bus (BH1750 library doesn't do this automatically)
    Wire.begin();
    lightMeter.begin();

    // Software to use pH for debugging
    // mod.begin(4800);

    /*START Servo Setup*/
    delay(250);
    myservo.attach(5);
    myservo.write(95);
    delay(1000);

    temp1 = analogRead(A1);
    delayMicroseconds(250);
    temp2 = analogRead(A2);
    delayMicroseconds(250);
    temp3 = temp2 - temp1;
    Serial.println(temp3);

    //sun 180 degree (Sunrise)
    if (temp3 <= -900)
    {
        servo_pos = 155;
        myservo.write(servo_pos);
        delay(500);
    }

    //between Sunrise and Noon
    if (temp3 >= -900 && temp3 <= -20)
    {
        servo_pos = 130;
        myservo.write(servo_pos);
        delay(500);
    }

    //sun 90 degree (Noon)
    if (temp3 >= -20 && temp3 <= 20)
    {
        servo_pos = 95;
        myservo.write(servo_pos);
        delay(500);
    }

    //between Noon and Susnet
    if (temp3 >= 20 && temp3 <= 900)
    {
        servo_pos = 60;
        myservo.write(servo_pos);
        delay(500);
    }

    //sun 0 degree (Sunset)
    if (temp3 >= 900)
    {
        servo_pos = 40;
        myservo.write(servo_pos);
        delay(500);
    }
    /*END Servo Setup*/
}

void loop()
{
    unsigned long currentMillis = millis();

    /*START Reading lx value*/
    if (currentMillis - bhMillis >= 100)
    {
        bhMillis = currentMillis;
        lux = lightMeter.readLightLevel();

        /*START Debugging Session*/

        // Serial.print("Light: ");
        // Serial.print(lux);
        // Serial.println(" lx");

        /*END Debugging Session*/
    }
    /*END Reading lx value*/

    /*START Reading pH Value*/
    if (currentMillis - phMillis >= 200)
    {
        phMillis = currentMillis;
        byte val;
        if (Serial.write(ph, sizeof(ph)) == 8)
        {
            for (byte i = 0; i < 11; i++)
            {
                values[i] = Serial.read();
            }
        }
        soil_ph = float(values[4]) / 10;

        /*START Debugging Session*/
        // Serial.print("Soil Ph: ");
        // Serial.println(soil_ph, 1);
    }
    /*END Reading pH Value*/

    /*START Control Lora Module*/
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

            if (inString == "2")
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
        LoRa.print(lux, 1);
        LoRa.print(",");
        // LoRa.print(soil_ph, 1);
        LoRa.print("7.0");
        LoRa.endPacket();

        if (currentMillis - startMillis_clearString >= clearString_interval)
        {
            startMillis_clearString = currentMillis;
            temp = 0;
        }
    }
    /*END Control Lora Module*/

    /*START Control Servo*/

    if (currentMillis - startMillis >= 50)
    {
        startMillis = currentMillis;
        temp1 = analogRead(A1);
        delayMicroseconds(250);
        temp2 = analogRead(A2);
        delayMicroseconds(250);
        temp3 = temp2 - temp1;
        // Serial.println(temp3);
    }

    if (currentMillis - startMillis1 >= 50)
    {
        startMillis1 = currentMillis;
        int tempx = temp3;
        if (tempx < -tolerance)
        {
            if (servo_pos < 155)
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

    /*END Control Servo*/
}

void servo_first_time()
{
    int temp11 = analogRead(A1);
    delay(100);
    int temp12 = analogRead(A2);
    delay(100);
    int temp13 = temp12 - temp11;

    if (temp13 <= -900)
    {
        // myservo.write(140);
        delay(25);
        int servo_pos1 = 140;
        myservo.write(servo_pos1);
    }

    if (temp13 > -300 && temp13 <= -200)
    {
        // myservo.write(120);
        delay(25);
        int servo_pos1 = 120;
        myservo.write(servo_pos1);
    }

    if (temp13 > -20 && temp13 <= 20)
    {
        // myservo.write(95);
        delay(25);
        int servo_pos1 = 95;
        myservo.write(servo_pos1);
    }

    if (temp13 > 250 && temp13 <= 300)
    {
        // myservo.write(70);
        delay(25);
        int servo_pos1 = 70;
        myservo.write(servo_pos1);
    }

    if (temp13 >= 900)
    {
        // myservo.write(30);
        delay(25);
        int servo_pos1 = 30;
        myservo.write(servo_pos1);
    }
}