/*
  Example of BH1750 library usage. This example initialises the BH1750 object using the default high resolution continuous mode and then makes a light level reading every second.
*/
#include <SoftwareSerial.h>
#include <Wire.h>
#include <BH1750.h>

const byte ph[] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x01, 0x84, 0x0A};
byte values[11];
SoftwareSerial mod(2, 3);

BH1750 lightMeter;
float lux = 0;

float soil_ph = 0;

unsigned long bhMillis = 0;
unsigned long phMillis = 0;

void setup()
{
    Serial.begin(9600);

    // Initialize the I2C bus (BH1750 library doesn't do this automatically)
    Wire.begin();
    lightMeter.begin();
    Serial.println(F("BH1750 Test begin"));

    // Software to use pH for debugging
    mod.begin(4800);
}

void loop()
{
    unsigned long currentMillis = millis();

    // Read BH1750 sensor every 100ms
    if (currentMillis - bhMillis >= 100)
    {
        bhMillis = currentMillis;
        lux = lightMeter.readLightLevel();
        Serial.print("Light: ");
        Serial.print(lux);
        Serial.println(" lx");
    }

    // Read pH sensor every 200ms
    if (currentMillis - phMillis >= 200)
    {
        phMillis = currentMillis;
        byte val;
        if (mod.write(ph, sizeof(ph)) == 8)
        {
            for (byte i = 0; i < 11; i++)
            {
                values[i] = mod.read();
                Serial.print(values[i], HEX);
            }
            Serial.println();
        }
        soil_ph = float(values[4]) / 10;
        Serial.print("Soil Ph: ");
        Serial.println(soil_ph, 1);
    }

    //Check node_1 examples
}