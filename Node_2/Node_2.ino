/*
  Example of BH1750 library usage. This example initialises the BH1750 object using the default high resolution continuous mode and then makes a light level reading every second.
*/
#include <SoftwareSerial.h>
#include <Wire.h>
#include <BH1750.h>
#include <SPI.h>
#include <LoRa.h>
#include <Servo.h>

#define tolerance 5
Servo myservo;
BH1750 lightMeter;

/*START Variables to control servo*/
unsigned int temp1, temp2;

signed int temp3;

unsigned long startMillis = 0;
const long interval = 10;

unsigned long startMillis1 = 0;
const long interval1 = 1000;

int servo_pos = 95;
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
    temp1 = 0;
    temp2 = 0;
    temp3 = 0;

    myservo.attach(5);
    myservo.write(servo_pos);

    delay(1000);

    // servo_first_time();
}

void loop()
{
    unsigned long currentMillis = millis();

    // Read BH1750 sensor every 100ms
    if (currentMillis - bhMillis >= 100)
    {
        bhMillis = currentMillis;
        lux = lightMeter.readLightLevel();
        // Serial.print("Light: ");
        // Serial.print(lux);
        // Serial.println(" lx");
    }

    // Read pH sensor every 200ms
    if (currentMillis - phMillis >= 200)
    {
        phMillis = currentMillis;
        byte val;
        if (Serial.write(ph, sizeof(ph)) == 8)
        {
            for (byte i = 0; i < 11; i++)
            {
                values[i] = Serial.read();
                // Serial.print(values[i], HEX);
            }
            // Serial.println();
        }
        soil_ph = float(values[4]) / 10;
        // Serial.print("Soil Ph: ");
        // Serial.println(soil_ph, 1);
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

    /*START Control Servo*/

    if (currentMillis - startMillis >= 50)
    {
        startMillis = currentMillis;
        temp1 = analogRead(A1);
        temp2 = analogRead(A2);
        // Serial.println(temp3);
        // temp3 = temp2 - temp1;
        // Serial.println(temp3);
    }

    if (currentMillis - startMillis1 >= 100)
    {
        startMillis1 = currentMillis;

        temp3 = temp2 - temp1;
        Serial.println(temp3);
        // myservo.write(servo_pos);

        // check if temp3 is still in range of -30 to 30 (or balance point)

        // if (temp3 <= -50)
        // {
        //     servo_pos = 135;
        //     rotate_servo_with_sun();
        // }

        // if (temp3 > -50 && temp3 <= -20)
        // {
        //     myservo.write(115);
        // }

        // if (temp3 > -10 && temp3 <= 10)
        // {
        //     myservo.write(90);
        // }

        // if (temp3 > 20 && temp3 <= 50)
        // {
        //     myservo.write(85);
        // }

        // if (temp3 >= 50)
        // {
        //     myservo.write(45);
        // }

        if (temp3 < -tolerance)
        {
            if (servo_pos < 180)
                servo_pos++;
        }

        if (temp3 > tolerance)
        {
            if (servo_pos > 0)
                servo_pos--;
        }

        myservo.write(servo_pos);
    }

    /*END Control Servo*/
}

void rotate_servo_with_sun()
{
    if (temp3 >= -5)
    {
        // servo will rotate to the last position that it's in
        servo_pos = servo_pos - 20;
        myservo.write(servo_pos);
    }

    return;
}

void servo_first_time()
{
    int temp11 = analogRead(A1);
    int temp12 = analogRead(A2);
    int temp13 = temp12 = temp11;

    if (temp13 <= -50)
    {
        myservo.write(135);
    }

    if (temp13 > -50 && temp13 <= -20)
    {
        myservo.write(115);
    }

    if (temp13 > -10 && temp13 <= 10)
    {
        myservo.write(90);
    }

    if (temp13 > 20 && temp13 <= 50)
    {
        myservo.write(85);
    }

    if (temp13 >= 50)
    {
        myservo.write(45);
    }
}