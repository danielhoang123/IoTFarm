#include <SPI.h>
#include <LoRa.h>
#include <Servo.h>

// Sensor pins
#define sensorMoist A0
#define sensorRain A3

Servo myservo;

unsigned int temp1, temp2;

signed int temp3;

unsigned long startMillis = 0;
const long interval = 10;

unsigned long startMillis1 = 0;
const long interval1 = 1000;

int valMoist;
int valRain;

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

    // servo
    myservo.attach(5);
    myservo.write(90);

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
}

void loop()
{
    // We using millis here
    unsigned long currentMillis = millis();

    // Read Moisture sensor every 500ms
    if (currentMillis - soilSensorMillis >= 500)
    {
        soilSensorMillis = currentMillis;
        valMoist = analogRead(sensorMoist);
        // Serial.print("Moisture: ");
        // Serial.println(valMoist);
    }

    // Read Moisture sensor every 500ms
    if (currentMillis - rainSensorMillis >= 500)
    {
        rainSensorMillis = currentMillis;
        valRain = analogRead(sensorRain);
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
        LoRa.print(valMoist);
        LoRa.print(",");
        LoRa.print(valRain);
        LoRa.endPacket();

        if (currentMillis - startMillis_clearString >= clearString_interval)
        {
            startMillis_clearString = currentMillis;
            temp = 0;
        }
    }

    // servo
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
}