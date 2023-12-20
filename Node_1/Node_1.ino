#include <SPI.h>
#include <LoRa.h>

// Sensor pins
#define sensorMoist A0
#define sensorRain A3

int valMoist;
int valRain;
int val = 0;

unsigned long soilSensorMillis = 0;
unsigned long rainSensorMillis = 0;
unsigned long receiveMillis = 0;

String inString = "";
bool modeSwitch = 0;

void setup()
{

    // For debugging
    Serial.begin(9600);

    // Init and debugging Lora's Initialization
    while (!Serial)
        ;
    Serial.println("LoRa Sender");
    if (!LoRa.begin(433E6))
    { // or 915E6, the MHz speed of yout module
        Serial.println("Starting LoRa failed!");
        while (1)
            ;
    }

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
        Serial.print("Moisture: ");
        Serial.println(valMoist);
    }

    // Read Moisture sensor every 500ms
    if (currentMillis - rainSensorMillis >= 500)
    {
        rainSensorMillis = currentMillis;
        valRain = analogRead(sensorRain);
        Serial.print("Rain: ");
        Serial.println(valRain);
    }

    if (modeSwitch == 0)
    {
        // For become a receiving module each 100ms to receiving ID "1"
        if (currentMillis - receiveMillis >= 200)
        {
            receiveMillis = currentMillis;
            // trying to parse packet
            int packetSize = LoRa.parsePacket();
            if (packetSize)
            {
                // read packet (có thể chuyển thành "while" thành "if" để xem thế nào)
                while (LoRa.available())
                {
                    int inChar = LoRa.read();
                    inString += (char)inChar;
                    val = inString.toInt();
                    if (inString == "1")
                    {
                        modeSwitch = 1;
                        Serial.println(modeSwitch);
                    }
                }
                inString = "";
                // LoRa.packetRssi();
            }
        }
    }

    // IF "1" Received (Viết cái này là chưa bao gồm gửi cờ chuyển mode cho LORA ESP
    if (modeSwitch == 1)
    {
        LoRa.beginPacket();
        LoRa.print(valMoist);
        LoRa.print(valRain);
        LoRa.endPacket();
        delay(50);
        modeSwitch = 0;
    }

}