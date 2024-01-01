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

unsigned long startMillis = 0;
const long interval = 10;

unsigned long startMillis1 = 0;
const long interval1 = 1000;

int servo_pos;
/*END Variables to control servo*/

/*START Variables to read soil's pH sensor*/

const byte ph[] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x01, 0x84, 0x0A};
byte values[11];
float soil_ph = 0;
unsigned long phMillis = 0;

/*END Variables to read soil's pH sensor*/

/*START Variables to read BH1750 sensor*/

float lux = 0.0;
unsigned long bhMillis = 0;

/*END Variables to read BH1750 sensor*/

/*START Variables to control Lora Module*/

unsigned long recMillis = 0; 
String inString = "";
bool temp = 0;
unsigned long startMillis_clearString = 0;
const long clearString_interval = 500;

/*END Variables to control Lora Module*/

void setup()
{
    //Init UART with baudrate 4800 to read value from pH sensor
    Serial.begin(4800);

    //Init Lora Module
    while (!LoRa.begin(433E6)) 
    {
        delay(500);
    }

    // Initialize the I2C bus (BH1750 library doesn't do this automatically)
    Wire.begin();
    lightMeter.begin();


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
        /*END Debugging Session*/
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