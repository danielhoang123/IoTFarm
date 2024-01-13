#include <SPI.h>
#include <LoRa.h>
#include <Servo.h>

#include <Wire.h>
#include "DFRobot_INA219.h"

DFRobot_INA219_IIC ina219(&Wire, INA219_I2C_ADDRESS4);

// Revise the following two paramters according to actula reading of the INA219 and the multimeter
// for linearly calibration
float ina219Reading_mA = 1000;
float extMeterReading_mA = 1000;

// Sensor pins
#define sensorMoist A0
#define sensorRain A3

#define tolerance 18
Servo myservo;

unsigned int temp1, temp2;

signed int temp3;

unsigned long startMillis = 0;
const long interval = 10;

unsigned long startMillis1 = 0;
const long interval1 = 50;
int servo_pos;

int valMoist;
int valRain;
int map_value = 0;

unsigned long soilSensorMillis = 0;
unsigned long rainSensorMillis = 0;
unsigned long receiveMillis = 0;

// String inString = "";
// bool modeSwitch = 0;

unsigned long recMillis = 0; // millis for changing to receiving mode
String inString = "";
int val = 0;
bool temp = 0;

unsigned long startMillis_clearString = 0;
const long clearString_interval = 500;

void setup()
{

    // For debugging
    Serial.begin(9600);

    // Init and debugging Lora's Initialization
    while (!Serial)
        ;

    if (!LoRa.begin(433E6))
    { // or 915E6, the MHz speed of yout module
        Serial.println("Starting LoRa failed!");
        while (1)
            ;
    }
    Serial.println("LoRa Sender");

    // Variables init
    valMoist = 0;
    valRain = 0;

    while (ina219.begin() != true)
    {
        Serial.println("INA219 begin faild");
        delay(2000);
    }
    ina219.linearCalibrate(ina219Reading_mA, extMeterReading_mA);

    /*START Servo Setup*/
    delay(250);
    myservo.attach(5);
    myservo.write(102);
    delay(1000);

    temp1 = analogRead(A2);
    delayMicroseconds(250);
    temp2 = analogRead(A1);
    delayMicroseconds(250);
    temp3 = temp2 - temp1;
    Serial.println(temp3);

    // sun 180 degree (Sunrise)
    if (temp3 <= -900)
    {
        servo_pos = 155;
        myservo.write(servo_pos);
        delay(500);
    }

    // between Sunrise and Noon
    if (temp3 >= -900 && temp3 <= -20)
    {
        servo_pos = 130;
        myservo.write(servo_pos);
        delay(500);
    }

    // sun 90 degree (Noon)
    if (temp3 >= -20 && temp3 <= 20)
    {
        servo_pos = 102;
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
}

void loop()
{
    // We using millis here
    unsigned long currentMillis = millis();

    // Read Moisture sensor every 500ms
    if (currentMillis - soilSensorMillis >= 100)
    {
        soilSensorMillis = currentMillis;
        valMoist = analogRead(sensorMoist);
        // Serial.println(valMoist);

        map_value = map(valMoist, 290, 480, 100, 0);

        if (map_value < 0)
        {
            map_value = 0;
        }
        // Serial.print("Moisture: ");
        // Serial.println(valMoist);
    }

    // Read Moisture sensor every 500ms
    if (currentMillis - rainSensorMillis >= 200)
    {
        rainSensorMillis = currentMillis;
        valRain = analogRead(sensorRain);

        Serial.print("BusVoltage:   ");
        Serial.print(ina219.getBusVoltage_V(), 2);
        Serial.println("V");
        // Serial.print("ShuntVoltage: ");
        // Serial.print(ina219.getShuntVoltage_mV(), 3);
        // Serial.println("mV");
        Serial.print("Current:      ");
        Serial.print(ina219.getCurrent_mA(), 1);
        Serial.println("mA");
        // Serial.print("Power:        ");
        // Serial.print(ina219.getPower_mW(), 1);
        // Serial.println("mW");
        Serial.println("");
        // Serial.print("Rain: ");
        // Serial.println(valRain);
    }

    // if (modeSwitch == 0)
    // {
    //     // For become a receiving module each 100ms to receiving ID "1"
    //     if (currentMillis - receiveMillis >= 200)
    //     {
    //         receiveMillis = currentMillis;
    //         // trying to parse packet
    //         int packetSize = LoRa.parsePacket();
    //         if (packetSize)
    //         {
    //             // read packet (có thể chuyển thành "while" thành "if" để xem thế nào)
    //             while (LoRa.available())
    //             {
    //                 int inChar = LoRa.read();
    //                 inString += (char)inChar;
    //                 val = inString.toInt();
    //                 if (inString == "1")
    //                 {
    //                     modeSwitch = 1;
    //                     Serial.println(modeSwitch);
    //                 }
    //             }
    //             inString = "";
    //             // LoRa.packetRssi();
    //         }
    //     }
    // }

    // // IF "1" Received (Viết cái này là chưa bao gồm gửi cờ chuyển mode cho LORA ESP
    // if (modeSwitch == 1)
    // {
    //     LoRa.beginPacket();
    //     LoRa.print(valMoist);
    //     LoRa.print(valRain);
    //     LoRa.endPacket();
    //     delay(50);
    //     modeSwitch = 0;
    // }

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

            if (inString == "1")
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
        LoRa.print(map_value);
        LoRa.print(",");
        LoRa.print(valRain);
        LoRa.print(";");
        LoRa.print(ina219.getBusVoltage_V(), 2);
        LoRa.print(":");
        LoRa.print(ina219.getCurrent_mA(), 1);
        LoRa.endPacket();

        if (currentMillis - startMillis_clearString >= clearString_interval)
        {
            startMillis_clearString = currentMillis;
            temp = 0;
        }
    }

    /*START Control Servo*/
    if (currentMillis - startMillis >= 50)
    {
        startMillis = currentMillis;
        temp1 = analogRead(A2);
        delayMicroseconds(250);
        temp2 = analogRead(A1);
        delayMicroseconds(250);
        temp3 = temp2 - temp1;
    }

    if (currentMillis - startMillis1 >= interval1)
    {
        startMillis1 = currentMillis;
        startMillis1 = currentMillis;
        int tempx = temp3;
        if (tempx < -tolerance)
        {
            if (servo_pos < 165)
            {
                myservo.write(servo_pos);
                servo_pos++;
            }
        }
        if (tempx > tolerance)
        {
            if (servo_pos > 35)
            {
                servo_pos--;
                myservo.write(servo_pos);
            }
        }
    }
    /*END Control Servo*/
}