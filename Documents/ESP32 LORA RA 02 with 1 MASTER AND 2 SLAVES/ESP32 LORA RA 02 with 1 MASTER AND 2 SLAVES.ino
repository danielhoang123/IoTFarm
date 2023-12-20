//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 01 Test Master ESP32 OLED Buttons
//---------------------------------------- Include Library.
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
//---------------------------------------- 

//---------------------------------------- Defines the Pins of the Buttons.
#define Button_1_Pin  13 //--> D13
#define Button_2_Pin  12 //--> D12
//---------------------------------------- 

//---------------------------------------- Configure OLED screen size in pixels.
#define SCREEN_WIDTH 128 //--> OLED display width, in pixels
#define SCREEN_HEIGHT 64 //--> OLED display height, in pixels
//----------------------------------------

//---------------------------------------- Declaration for an SSD1306 display connected to I2C (SDA, SCL pins).
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
//----------------------------------------

// Variable declaration for buttons.
byte Button_1_State, Button_2_State;

//________________________________________________________________________________ VOID SETUP
void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);

  pinMode(Button_1_Pin, INPUT);
  pinMode(Button_2_Pin, INPUT);

  //---------------------------------------- SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally.
  // Address 0x3C for 128x32 and Address 0x3D for 128x64.
  // But on my 128x64 module the 0x3D address doesn't work. What works is the 0x3C address.
  // So please try which address works on your module.
  Serial.println();
  Serial.println("Start OLED SSD1306 init...");
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); //--> Don't proceed, loop forever
  }
  Serial.println("OLED SSD1306 init succeeded.");
  //----------------------------------------
}
//________________________________________________________________________________ 

//________________________________________________________________________________ VOID LOOP
void loop() {
  // put your main code here, to run repeatedly:

  Button_1_State = digitalRead(Button_1_Pin);
  Button_2_State = digitalRead(Button_2_Pin);

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0); //--> (x position, y position)
  display.print("Button 1 : ");
  display.print(Button_1_State);
  display.setCursor(0, 10);
  display.print("Button 2 : ");
  display.print(Button_2_State);
  display.display(); 
  delay(100);
}
//________________________________________________________________________________ 
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<



#################################################################################################
#################################################################################################
#################################################################################################
################################################################################################# 



//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 02 Test Slave 1 and Slave 2 ESP32 DHT11 LED
//---------------------------------------- Include Library.
#include "DHT.h"
//---------------------------------------- 

//---------------------------------------- Defines the DHT11 Pin and the DHT type.
#define DHTPIN      15
#define DHTTYPE     DHT11
//---------------------------------------- 

//---------------------------------------- Defines LED Pins.
#define LED_1_Pin   27
//---------------------------------------- 

// Initializes the DHT sensor (DHT11).
DHT dht11(DHTPIN, DHTTYPE);

//---------------------------------------- Variable declarations for temperature and humidity values.
int h;
float t;
//---------------------------------------- 

//________________________________________________________________________________ VOID SETUP
void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);

  pinMode(LED_1_Pin, OUTPUT);

  digitalWrite(LED_1_Pin, HIGH);
  delay(2000);
  digitalWrite(LED_1_Pin, LOW);
  delay(2000);
  
  dht11.begin();
}
//________________________________________________________________________________ 

//________________________________________________________________________________ VOID LOOP
void loop() {
  // put your main code here, to run repeatedly:

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  h = dht11.readHumidity();
  // Read temperature as Celsius (the default)
  t = dht11.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  //float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.println(F("°C "));

  digitalWrite(LED_1_Pin, !digitalRead(LED_1_Pin));

  delay(2000);
}
//________________________________________________________________________________ 
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<



#################################################################################################
#################################################################################################
#################################################################################################
################################################################################################# 



//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 03 Test Master ESP32 Lora Ra-02

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  --------------------------------------                                                                                                            //
// Description of the communication method between the Master and the Slaves.                                                                         //
//                                                                                                                                                    //
// > The master will send a message containing command data to the slaves to send temperature and humidity data from the DHT11 sensor to the master.  //
//                                                                                                                                                    //
// > The Master will send messages containing command data to Slaves alternately to Slave 1 and Slave 2.                                              //
//   This is done to avoid "collisions" when Slave 1 and Slave 2 send data back to the Master.                                                        //
//  --------------------------------------                                                                                                            //
//                                                                                                                                                    //
//  --------------------------------------                                                                                                            //
//  Additional information :                                                                                                                          //
//                                                                                                                                                    //
//  If you are sure that you have connected the Lora Ra-02 correctly to the ESP32,                                                                    //
//  but you get the error message "LoRa init failed ! Check your connections", then try using an external power source for the Lora Ra-02.            //
//  In this project I used a 3.3V power supply from an ESP32 board to power the Lora Ra-02.                                                           //
//  Because maybe the quality of the ESP32 board module is different in the market and the power supply is not strong enough to power the Lora Ra-02. //
//  So you get the error message "LoRa init failed ! Check your connections".                                                                         //
//  --------------------------------------                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------- Include Library.
#include <SPI.h>
#include <LoRa.h>
//----------------------------------------

//---------------------------------------- LoRa Pin / GPIO configuration.
#define ss 5
#define rst 14
#define dio0 2
//----------------------------------------

//---------------------------------------- Variable declaration to hold incoming and outgoing data.
String Incoming = "";
String Message = "";            
//----------------------------------------

//---------------------------------------- LoRa data transmission configuration.
byte LocalAddress = 0x01;               //--> address of this device (Master Address).
byte Destination_ESP32_Slave_1 = 0x02;  //--> destination to send to Slave 1 (ESP32).
byte Destination_ESP32_Slave_2 = 0x03;  //--> destination to send to Slave 2 (ESP32).
//---------------------------------------- 

//---------------------------------------- Variable declaration for Millis/Timer.
unsigned long previousMillis_SendMSG = 0;
const long interval_SendMSG = 1000;
//---------------------------------------- 

// Variable declaration to count slaves.
byte Slv = 0;

//________________________________________________________________________________ Subroutines for sending data (LoRa Ra-02).
void sendMessage(String Outgoing, byte Destination) {
  LoRa.beginPacket();             //--> start packet
  LoRa.write(Destination);        //--> add destination address
  LoRa.write(LocalAddress);       //--> add sender address
  LoRa.write(Outgoing.length());  //--> add payload length
  LoRa.print(Outgoing);           //--> add payload
  LoRa.endPacket();               //--> finish packet and send it
}
//________________________________________________________________________________ 

//________________________________________________________________________________ Subroutines for receiving data (LoRa Ra-02).
void onReceive(int packetSize) {
  if (packetSize == 0) return;  //--> if there's no packet, return

  //---------------------------------------- read packet header bytes:
  int recipient = LoRa.read();        //--> recipient address
  byte sender = LoRa.read();          //--> sender address
  byte incomingLength = LoRa.read();  //--> incoming msg length
  //---------------------------------------- 

  // Clears Incoming variable data.
  Incoming = "";

  //---------------------------------------- Get all incoming data.
  while (LoRa.available()) {
    Incoming += (char)LoRa.read();
  }
  //---------------------------------------- 

  //---------------------------------------- Check length for error.
  if (incomingLength != Incoming.length()) {
    Serial.println("error: message length does not match length");
    return; //--> skip rest of function
  }
  //---------------------------------------- 

  //---------------------------------------- Checks whether the incoming data or message for this device.
  if (recipient != LocalAddress) {
    Serial.println("This message is not for me.");
    return; //--> skip rest of function
  }
  //---------------------------------------- 

  //---------------------------------------- if message is for this device, or broadcast, print details:
  Serial.println();
  Serial.println("Received from: 0x" + String(sender, HEX));
  Serial.println("Message length: " + String(incomingLength));
  Serial.println("Message: " + Incoming);
  //Serial.println("RSSI: " + String(LoRa.packetRssi()));
  //Serial.println("Snr: " + String(LoRa.packetSnr()));
  //---------------------------------------- 
}
//________________________________________________________________________________ 

//________________________________________________________________________________ VOID SETUP
void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);

  //---------------------------------------- Settings and start Lora Ra-02.
  LoRa.setPins(ss, rst, dio0);

  Serial.println("Start LoRa init...");
  if (!LoRa.begin(433E6)) {             // initialize ratio at 915 or 433 MHz
    Serial.println("LoRa init failed. Check your connections.");
    while (true);                       // if failed, do nothing
  }
  Serial.println("LoRa init succeeded.");
  //---------------------------------------- 
}
//________________________________________________________________________________ 

//________________________________________________________________________________ VOID LOOP
void loop() {
  // put your main code here, to run repeatedly:

  //---------------------------------------- Millis or Timer to send message / command data to slaves every 1 second (see interval_SendCMD variable).
  // Messages are sent every one second is alternately.
  // > Master sends message to Slave 1, delay 1 second.
  // > Master sends message to Slave 2, delay 1 second.
  
  unsigned long currentMillis_SendMSG = millis();
  
  if (currentMillis_SendMSG - previousMillis_SendMSG >= interval_SendMSG) {
    previousMillis_SendMSG = currentMillis_SendMSG;

    Slv++;
    if (Slv > 2) Slv = 1;

    Message = "SDS" + String(Slv);

    //::::::::::::::::: Condition for sending message / command data to Slave 1 (ESP32 Slave 1).
    if (Slv == 1) {
      Serial.println();
      Serial.print("Send message to ESP32 Slave " + String(Slv));
      Serial.println(" : " + Message);
      sendMessage(Message, Destination_ESP32_Slave_1);
    }
    //:::::::::::::::::

    //::::::::::::::::: Condition for sending message / command data to Slave 2 (UNO Slave 2).
    if (Slv == 2) {
      Serial.println();
      Serial.print("Send message to ESP32 Slave " + String(Slv));
      Serial.println(" : " + Message);
      sendMessage(Message, Destination_ESP32_Slave_2);
    }
    //:::::::::::::::::
  }
  //----------------------------------------

  //---------------------------------------- parse for a packet, and call onReceive with the result:
  onReceive(LoRa.parsePacket());
  //----------------------------------------
}
//________________________________________________________________________________ 
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<



#################################################################################################
#################################################################################################
#################################################################################################
################################################################################################# 



//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 04 Test Slave (1 or 2) ESP32 Lora Ra-02

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  --------------------------------------                                                                                                            //
// Description of the communication method between the Master and the Slaves.                                                                         //
//                                                                                                                                                    //
// > The master will send a message containing command data to the slaves to send temperature and humidity data from the DHT11 sensor to the master.  //
//                                                                                                                                                    //
// > The Master will send messages containing command data to Slaves alternately to Slave 1 and Slave 2.                                              //
//   This is done to avoid "collisions" when Slave 1 and Slave 2 send data back to the Master.                                                        //
//  --------------------------------------                                                                                                            //
//                                                                                                                                                    //
//  --------------------------------------                                                                                                            //
//  Additional information :                                                                                                                          //
//                                                                                                                                                    //
//  If you are sure that you have connected the Lora Ra-02 correctly to the ESP32,                                                                    //
//  but you get the error message "LoRa init failed ! Check your connections", then try using an external power source for the Lora Ra-02.            //
//  In this project I used a 3.3V power supply from an ESP32 board to power the Lora Ra-02.                                                           //
//  Because maybe the quality of the ESP32 board module is different in the market and the power supply is not strong enough to power the Lora Ra-02. //
//  So you get the error message "LoRa init failed ! Check your connections".                                                                         //
//  --------------------------------------                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------- Include Library.
#include <SPI.h>
#include <LoRa.h>
#include "DHT.h"
//---------------------------------------- 

//---------------------------------------- Defines the DHT11 Pin and the DHT type.
#define DHTPIN      15
#define DHTTYPE     DHT11
//---------------------------------------- 

//---------------------------------------- Defines LED Pins.
#define LED_1_Pin   27
//---------------------------------------- 

//---------------------------------------- LoRa Pin / GPIO configuration.
#define ss 5
#define rst 14
#define dio0 2
//----------------------------------------

// Initializes the DHT sensor (DHT11).
DHT dht11(DHTPIN, DHTTYPE);

//---------------------------------------- Variable declaration to hold incoming and outgoing data.
String Incoming = "";
String Message = "";             
//----------------------------------------

//---------------------------------------- LoRa data transmission configuration.
////////////////////////////////////////////////////////////////////////////
// PLEASE UNCOMMENT AND SELECT ONE OF THE "LocalAddress" VARIABLES BELOW. //
////////////////////////////////////////////////////////////////////////////

//byte LocalAddress = 0x02;       //--> address of this device (Slave 1).
//byte LocalAddress = 0x03;       //--> address of this device (Slave 2).

byte Destination_Master = 0x01; //--> destination to send to Master (ESP32).
//----------------------------------------

//---------------------------------------- Variable declarations for temperature and humidity values.
int h = 0;
float t = 0.0;
//---------------------------------------- 

//---------------------------------------- Millis / Timer to update temperature and humidity values from DHT11 sensor.
unsigned long previousMillis_UpdateDHT11 = 0;
const long interval_UpdateDHT11 = 2000;
//---------------------------------------- 

//________________________________________________________________________________ Subroutines for sending data (LoRa Ra-02).
void sendMessage(String Outgoing, byte Destination) {
  LoRa.beginPacket();             //--> start packet
  LoRa.write(Destination);        //--> add destination address
  LoRa.write(LocalAddress);       //--> add sender address
  LoRa.write(Outgoing.length());  //--> add payload length
  LoRa.print(Outgoing);           //--> add payload
  LoRa.endPacket();               //--> finish packet and send it
}
//________________________________________________________________________________ 

//________________________________________________________________________________ Subroutines for receiving data (LoRa Ra-02).
void onReceive(int packetSize) {
  if (packetSize == 0) return;  //--> if there's no packet, return

  //---------------------------------------- read packet header bytes:
  int recipient = LoRa.read();        //--> recipient address
  byte sender = LoRa.read();          //--> sender address
  byte incomingLength = LoRa.read();  //--> incoming msg length
  //---------------------------------------- 

  // Clears Incoming variable data.
  Incoming = "";

  //---------------------------------------- Get all incoming data.
  while (LoRa.available()) {
    Incoming += (char)LoRa.read();
  }
  //---------------------------------------- 

  //---------------------------------------- Check length for error.
  if (incomingLength != Incoming.length()) {
    Serial.println("error: message length does not match length");
    return; //--> skip rest of function
  }
  //---------------------------------------- 

  //---------------------------------------- Checks whether the incoming data or message for this device.
  if (recipient != LocalAddress) {
    Serial.println("This message is not for me.");
    return; //--> skip rest of function
  }
  //---------------------------------------- 

  //---------------------------------------- if message is for this device, or broadcast, print details:
  Serial.println();
  Serial.println("Received from: 0x" + String(sender, HEX));
  Serial.println("Message length: " + String(incomingLength));
  Serial.println("Message: " + Incoming);
  //Serial.println("RSSI: " + String(LoRa.packetRssi()));
  //Serial.println("Snr: " + String(LoRa.packetSnr()));
  //---------------------------------------- 

  // Calls the Processing_incoming_data() subroutine.
  Processing_incoming_data();
}
//________________________________________________________________________________ 

//________________________________________________________________________________ Subroutines to process data from incoming messages, then send messages to the Master.
void Processing_incoming_data() {
  //---------------------------------------- Conditions for sending messages to Master.
/////////////////////////////////////////////////////////////////////////////////////////
// PLEASE UNCOMMENT THE LINE OF CODE BELOW IF THIS CODE OR THIS DEVICE IS FOR SLAVE 1. //
/////////////////////////////////////////////////////////////////////////////////////////

//  if (Incoming == "SDS1") {
//    Message = "SL1," + String(h) + "," + String(t);
//
//    Serial.println();
//    Serial.print("Send message to Master : ");
//    Serial.println(Message);
//    
//    sendMessage(Message, Destination_Master);
//  }
  //---------------------------------------- 

  //---------------------------------------- Conditions for sending messages to Master.
/////////////////////////////////////////////////////////////////////////////////////////
// PLEASE UNCOMMENT THE LINE OF CODE BELOW IF THIS CODE OR THIS DEVICE IS FOR SLAVE 2. //
/////////////////////////////////////////////////////////////////////////////////////////

//  if (Incoming == "SDS2") {
//    Message = "SL2," + String(h) + "," + String(t);
//
//    Serial.println();
//    Serial.print("Send message to Master : ");
//    Serial.println(Message);
//    
//    sendMessage(Message, Destination_Master);
//  }
  //---------------------------------------- 
}
//________________________________________________________________________________ 

//________________________________________________________________________________ VOID SETUP
void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);

  dht11.begin();

  //---------------------------------------- Settings and start Lora Ra-02.
  LoRa.setPins(ss, rst, dio0);
  
  Serial.println();
  Serial.println("Start LoRa init...");
  if (!LoRa.begin(433E6)) {             // initialize ratio at 915 or 433 MHz
    Serial.println("LoRa init failed. Check your connections.");
    while (true);                       // if failed, do nothing
  }
  Serial.println("LoRa init succeeded.");
  //---------------------------------------- 
}
//________________________________________________________________________________ 

//________________________________________________________________________________ VOID LOOP
void loop() {
  // put your main code here, to run repeatedly:

  //---------------------------------------- Millis / Timer to update the temperature and humidity values ​​from the DHT11 sensor every 2 seconds (see the variable interval_UpdateDHT11).
  unsigned long currentMillis_UpdateDHT11 = millis();
  
  if (currentMillis_UpdateDHT11 - previousMillis_UpdateDHT11 >= interval_UpdateDHT11) {
    previousMillis_UpdateDHT11 = currentMillis_UpdateDHT11;

    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    h = dht11.readHumidity();
    // Read temperature as Celsius (the default)
    t = dht11.readTemperature();
    // Read temperature as Fahrenheit (isFahrenheit = true)
    //float f = dht.readTemperature(true);
  
    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t)) {
      Serial.println(F("Failed to read from DHT sensor!"));
      return;
    }
  }
  //---------------------------------------- 
  
  //---------------------------------------- parse for a packet, and call onReceive with the result:
  onReceive(LoRa.parsePacket());
  //----------------------------------------
}
//________________________________________________________________________________ 
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<



#################################################################################################
#################################################################################################
#################################################################################################
################################################################################################# 



//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 05 ESP32 Master OLED Buttons Lora Ra-02 Final

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  --------------------------------------                                                                                                            //
//  Description of how this project works.                                                                                                            //
//                                                                                                                                                    //
//  - The master sends a broadcast message to the slaves with a certain address every 1 second.                                                       //
//                                                                                                                                                    //
//  - The contents of the broadcast message from the Master are: Slave_Address/Destination | Address_Master/Sender | Message/Data                     //
//    (Message/Data contains commands to control the LEDs on the slaves.)                                                                             //
//                                                                                                                                                    //
//  - Example :                                                                                                                                       //
//    Broadcast message content : 0x02 | 0x01 | f,f                                                                                                   //
//    > 0x02 = Slave_Address/Destination.                                                                                                             //
//    > 0x01 = Master/Sender_Address.                                                                                                                 //
//    > f,f = "f" the first one to control the LED on Slave 1,"f' the second one to control the LED on Slave 2.                                       //
//    > "t" to turn on the LED, "f" to turn off the LED.                                                                                              //
//                                                                                                                                                    //
//  - The slaves received a broadcast message from the master.                                                                                        //
//                                                                                                                                                    //
//  - The Slaves will check whether the broadcast message sent by the Master is for Slave 1 or Slave 2.                                               //
//                                                                                                                                                    //
//  - If the broadcast message sent by the Master is not for the intended slave, then the slave will ignore the broadcast message,                    //
//    but the content of the broadcast message is still retrieved for processing,                                                                     //
//    because the content of the broadcast message is to control the LEDs on the slaves.                                                              //
//                                                                                                                                                    //
//  - If the broadcast message sent by the Master is indeed for the intended slave,                                                                   //
//    then the slave will take the contents of the broadcast message to be processed,                                                                 //
//    because the content of the broadcast message is to control the LEDs on the slaves,                                                              //
//    then the slave will send a message back to the Master containing humidity value, temperature value and the last state of the LED.               //
//  --------------------------------------                                                                                                            //
//                                                                                                                                                    //
//  --------------------------------------                                                                                                            //
//  An overview of the workings of communication between the Master and the Slaves.                                                                   //
//                                                                                                                                                    //
//  - Master sends a broadcast message with the destination address is Slave 1.                                                                       //
//  - 1 second delay.                                                                                                                                 //
//  - Slave 1 receives a broadcast message,                                                                                                           //
//    Slave 1 processes the contents of the broadcast message to take orders to control the LED, because the message is intended for Slave 1,         //
//    Slave 1 sends a message back to the Master containing the data on humidity, temperature and the last state of the LED.                          //
//  - Slave 2 receives the broadcast message,                                                                                                         //
//    Slave 2 processes the contents of the broadcast message to take the command to control the LED,                                                 //
//    because the message is not intended for Slave 2, it does not send a message back to the Master.                                                 //
//  - Master received a message back from Slave 1.                                                                                                    //
//                                                                                                                                                    //
//  - Master sends a broadcast message with the destination address is Slave 2.                                                                       //
//  - 1 second delay.                                                                                                                                 //
//  - Slave 1 receives the broadcast message,                                                                                                         //
//    Slave 1 processes the contents of the broadcast message to take the command to control the LED,                                                 //
//    because the message is not intended for Slave 1, it does not send a message back to the Master.                                                 //
//  - Slave 2 receives a broadcast message,                                                                                                           //
//    Slave 2 processes the contents of the broadcast message to take orders to control the LED, because the message is intended for Slave 2,         //
//    then Slave 2 sends a message back to the Master containing data on humidity, temperature and the last state of the LED.                         //
//  - Master received a message back from Slave 2.                                                                                                    //
//  --------------------------------------                                                                                                            //
//                                                                                                                                                    //
//  --------------------------------------                                                                                                            //
//  Additional information :                                                                                                                          //
//                                                                                                                                                    //
//  If you are sure that you have connected the Lora Ra-02 correctly to the ESP32,                                                                    //
//  but you get the error message "LoRa init failed ! Check your connections", then try using an external power source for the Lora Ra-02.            //
//  In this project I used a 3.3V power supply from an ESP32 board to power the Lora Ra-02.                                                           //
//  Because maybe the quality of the ESP32 board module is different in the market and the power supply is not strong enough to power the Lora Ra-02. //
//  So you get the error message "LoRa init failed ! Check your connections".                                                                         //
//  --------------------------------------                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------- Include Library.
#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
//---------------------------------------- 

//---------------------------------------- Defines the Pins of the Buttons.
#define Button_1_Pin  13 //--> D13
#define Button_2_Pin  12 //--> D12
//---------------------------------------- 

//---------------------------------------- Configure OLED screen size in pixels.
#define SCREEN_WIDTH 128 //--> OLED display width, in pixels
#define SCREEN_HEIGHT 64 //--> OLED display height, in pixels
//----------------------------------------

//---------------------------------------- Declaration for an SSD1306 display connected to I2C (SDA, SCL pins).
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
//----------------------------------------

//---------------------------------------- LoRa Pin / GPIO configuration.
#define ss 5
#define rst 14
#define dio0 2
//----------------------------------------

//---------------------------------------- Variable declaration to hold incoming and outgoing data.
String Incoming = "";
String Message = "";             
//----------------------------------------

//---------------------------------------- LoRa data transmission configuration.
byte LocalAddress = 0x01;               //--> address of this device (Master Address).
byte Destination_ESP32_Slave_1 = 0x02;  //--> destination to send to Slave 1 (ESP32).
byte Destination_ESP32_Slave_2 = 0x03;  //--> destination to send to Slave 2 (ESP32).
//---------------------------------------- 

//---------------------------------------- Variable declaration for Millis/Timer.
unsigned long previousMillis_SendMSG = 0;
const long interval_SendMSG = 1000;

unsigned long previousMillis_RestartLORA = 0;
const long interval_RestartLORA = 1000;
//---------------------------------------- 

//---------------------------------------- Declaration of helper variables for the operation of the buttons.
bool Trig_Button_1_State = false;
bool Trig_Button_2_State = false;
//---------------------------------------- 

//---------------------------------------- Variable declaration to control the LEDs on the Slaves.
bool LED_1_State_Slave_1 = false;
bool LED_1_State_Slave_2 = false;
//---------------------------------------- 

//---------------------------------------- Variable declaration to display the state of the LEDs on the Slaves in the OLED LCD.
bool LED_1_State_Disp_Slave_1 = false;
bool LED_1_State_Disp_Slave_2 = false;
//---------------------------------------- 

//---------------------------------------- Variable declaration to get temperature and humidity values received from Slaves.
int Humd[2];
float Temp[2];
//---------------------------------------- 

// Variable declaration to count slaves.
byte Slv = 0;

// Declare a variable as a counter to update the OLED LCD display if there is no incoming message from the slaves.
byte Count_OLED_refresh_when_no_data_comes_in = 0;

// Variable declaration to get the address of the slaves.
byte SL_Address;

// Variable declaration to get LED state on slaves.
String LEDs_State = "";

// Declaration of variable as counter to restart Lora Ra-02.
byte Count_to_Rst_LORA = 0;

//________________________________________________________________________________ Subroutines for sending data (LoRa Ra-02).
void sendMessage(String Outgoing, byte Destination) {
  LoRa.beginPacket();             //--> start packet
  LoRa.write(Destination);        //--> add destination address
  LoRa.write(LocalAddress);       //--> add sender address
  LoRa.write(Outgoing.length());  //--> add payload length
  LoRa.print(Outgoing);           //--> add payload
  LoRa.endPacket();               //--> finish packet and send it
}
//________________________________________________________________________________ 

//________________________________________________________________________________ Subroutines for receiving data (LoRa Ra-02).
void onReceive(int packetSize) {
  if (packetSize == 0) return;          // if there's no packet, return

  //---------------------------------------- read packet header bytes:
  int recipient = LoRa.read();        //--> recipient address
  byte sender = LoRa.read();          //--> sender address
  byte incomingLength = LoRa.read();  //--> incoming msg length
  //---------------------------------------- 

  // Get the address of the senders or slaves.
  SL_Address = sender;

  // Clears Incoming variable data.
  Incoming = "";

  //---------------------------------------- Get all incoming data / message.
  while (LoRa.available()) {
    Incoming += (char)LoRa.read();
  }
  //---------------------------------------- 

  // Resets the value of the Count_to_Rst_LORA variable if a message is received.
  Count_to_Rst_LORA = 0;

  // Reset the value of the Count_OLED_refresh_when_no_data_comes_in variable if a message is received.
  Count_OLED_refresh_when_no_data_comes_in = 0;

  //---------------------------------------- Check length for error.
  if (incomingLength != Incoming.length()) {
    Serial.println();
    Serial.println("er"); //--> "er" = error: message length does not match length.
    //Serial.println("error: message length does not match length");
    return; //--> skip rest of function
  }
  //---------------------------------------- 

  //---------------------------------------- Checks whether the incoming data or message for this device.
  if (recipient != LocalAddress) {
    Serial.println();
    Serial.println("!");  //--> "!" = This message is not for me.
    //Serial.println("This message is not for me.");
    return; //--> skip rest of function
  }
  //---------------------------------------- 

  //----------------------------------------  if message is for this device, or broadcast, print details:
  Serial.println();
  Serial.println("Rc from: 0x" + String(sender, HEX));
  Serial.println("Message: " + Incoming);
  //---------------------------------------- 

  // Calls the Processing_incoming_data() subroutine.
  Processing_incoming_data();
}
//________________________________________________________________________________ 

//________________________________________________________________________________ Subroutines to process data from incoming messages.
//  - The message sent by the slaves contained data like this: Humidity_Value,Temperature_Value,Last_State of_LED
//
//  - Example : 80,30.50,0
//    > 80 is the humidity value.
//    > 30.50 is the temperature value.
//    > 0 is the last state of the LED (0 means the LED is off and 1 means the LED is on).
//
//  - To separate data as in the example above, the "GetValue" subroutine is used, using the separator ",".
// 
//  - Example :
//    Incoming = "80,30.50,0".
//    > 80 is the first data or "0" if using the "GetValue" subroutine.
//    > 30.50 is the second data or "1" if using the "GetValue" subroutine.
//    > 0 is the third data or "2" if using the "GetValue" subroutine.
//
//  - So to retrieve the data above is like this:
//    > Humdity = GetValue(Incoming, ',', 0).toInt();
//    > Temperature = GetValue(Incoming, ',', 1).toFloat();
//    > LEDs_State = GetValue(Incoming, ',', 2);

void Processing_incoming_data() {
  //---------------------------------------- Conditions for processing data or messages from Slave 1 (ESP32 Slave 1).
  if (SL_Address == Destination_ESP32_Slave_1) {
    Humd[0] = GetValue(Incoming, ',', 0).toInt();
    Temp[0] = GetValue(Incoming, ',', 1).toFloat();
    LEDs_State = GetValue(Incoming, ',', 2);
    if (LEDs_State == "1" || LEDs_State == "0") {
      LED_1_State_Slave_1 = LEDs_State.toInt();
      LED_1_State_Disp_Slave_1 = LEDs_State.toInt();
    }
  }
  //---------------------------------------- 

  //---------------------------------------- Conditions for processing data or messages from Slave 2 (ESP32 Slave 2).
  if (SL_Address == Destination_ESP32_Slave_2) {
    Humd[1] = GetValue(Incoming, ',', 0).toInt();
    Temp[1] = GetValue(Incoming, ',', 1).toFloat();
    LEDs_State = GetValue(Incoming, ',', 2);
    if (LEDs_State == "1" || LEDs_State == "0") {
      LED_1_State_Slave_2 = LEDs_State.toInt();
      LED_1_State_Disp_Slave_2 = LEDs_State.toInt();
    }
  }
  //---------------------------------------- 

  // Calls the Update_OLED_Display() subroutine.
  Update_OLED_Display();
}
//________________________________________________________________________________ 

//________________________________________________________________________________ Subroutine to display message content from slaves on OLED LCD.
void Update_OLED_Display() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.print("Slave 1 (ESP32)");
  display.setCursor(0, 10);
  display.print("T = ");
  display.print(Temp[0]);
  display.print(" ");
  display.print((char)247); //--> ASCII degree symbol
  display.print("C");
  display.setCursor(80, 10);
  display.print("H = ");
  display.print(Humd[0]);
  display.print(" %");
  display.setCursor(0, 20);
  display.print("LED_1 = ");
  if (LED_1_State_Disp_Slave_1 == true) display.print("ON");
  if (LED_1_State_Disp_Slave_1 == false) display.print("OFF");
  display.setCursor(0, 35);
  display.print("Slave 2 (ESP32)");
  display.setCursor(0, 45);
  display.print("T = ");
  display.print(Temp[1]);
  display.print(" ");
  display.print((char)247); //--> ASCII degree symbol
  display.print("C");
  display.setCursor(80, 45);
  display.print("H = ");
  display.print(Humd[1]);
  display.print(" %");
  display.setCursor(0, 55);
  display.print("LED_1 = ");
  if (LED_1_State_Disp_Slave_2 == true) display.print("ON");
  if (LED_1_State_Disp_Slave_2 == false) display.print("OFF");
  display.display();

  // Clears Incoming variable data.
  Incoming = "";
}
//________________________________________________________________________________ 

//________________________________________________________________________________ Subroutine to get messages from slaves containing the latest data.
//  - The master sends a message to the slaves.
//  - The slaves reply to the message from the master by sending a message containing the latest data of temperature, humidity and LED state to the Master.

void Getting_data_for_the_first_time() {
  Serial.println();
  Serial.println("Getting data for the first time...");

  //---------------------------------------- Loop to get data for the first time.
  while(true) {
    unsigned long currentMillis_SendMSG = millis();

    if (currentMillis_SendMSG - previousMillis_SendMSG >= interval_SendMSG) {
      previousMillis_SendMSG = currentMillis_SendMSG;

      Slv++;
      if (Slv > 2) {
        Slv = 0;
        Serial.println();
        Serial.println("Getting data for the first time has been completed.");
        break;
      }
  
      Message = "N,N";

      //::::::::::::::::: Condition for sending message / command data to Slave 1 (ESP32 Slave 1).
      if (Slv == 1) {
        Serial.println();
        Serial.print("Send message to ESP32 Slave " + String(Slv));
        Serial.println(" for first time : " + Message);
        sendMessage(Message, Destination_ESP32_Slave_1);
      }
      //::::::::::::::::: 

      //::::::::::::::::: Condition for sending message / command data to Slave 2 (ESP32 Slave 2).
      if (Slv == 2) {
        Serial.println();
        Serial.print("Send message to ESP32 Slave " + String(Slv));
        Serial.println(" for first time : " + Message);
        sendMessage(Message, Destination_ESP32_Slave_2);
      }
      //::::::::::::::::: 
    }

    //---------------------------------------- parse for a packet, and call onReceive with the result:
    onReceive(LoRa.parsePacket());
    //----------------------------------------
  }
  //---------------------------------------- 
}
//________________________________________________________________________________ 

//________________________________________________________________________________ String function to process the data received
// I got this from : https://www.electroniclinic.com/reyax-lora-based-multiple-sensors-monitoring-using-arduino/
String GetValue(String data, char separator, int index) {
  int found = 0;
  int strIndex[] = { 0, -1 };
  int maxIndex = data.length() - 1;
  
  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}
//________________________________________________________________________________ 

//________________________________________________________________________________ Subroutine to check the state of buttons.
bool Button(int btn_num) {
  bool Button_Result;
  if (btn_num == 1) Button_Result = digitalRead(Button_1_Pin);
  if (btn_num == 2) Button_Result = digitalRead(Button_2_Pin);
  return Button_Result;
}
//________________________________________________________________________________ 

//________________________________________________________________________________ Subroutine to reset Lora Ra-02.
void Rst_LORA() {
  LoRa.setPins(ss, rst, dio0);

  Serial.println();
  Serial.println("Restart LoRa...");
  Serial.println("Start LoRa init...");
  if (!LoRa.begin(433E6)) {             // initialize ratio at 915 or 433 MHz
    Serial.println("LoRa init failed. Check your connections.");
    while (true);                       // if failed, do nothing
  }
  Serial.println("LoRa init succeeded.");

  // Reset the value of the Count_to_Rst_LORA variable.
  Count_to_Rst_LORA = 0;
}
//________________________________________________________________________________ 

//________________________________________________________________________________ VOID SETUP
void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);

  pinMode(Button_1_Pin, INPUT);
  pinMode(Button_2_Pin, INPUT);

  //---------------------------------------- SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally.
  // Address 0x3C for 128x32 and Address 0x3D for 128x64.
  // But on my 128x64 module the 0x3D address doesn't work. What works is the 0x3C address.
  // So please try which address works on your module.
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); //--> Don't proceed, loop forever
  }
  //----------------------------------------

  //---------------------------------------- 
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(32, 15);
  display.println("ESP32 LORA");
  display.setCursor(17, 35);
  display.println("WEATHER STATION");
  display.display();
  //---------------------------------------- 

  delay(2000);

  //---------------------------------------- Clears the values of the Temp and Humd array variables for the first time.
  for (byte i = 0; i < 2; i++) {
    Humd[i] = 0;
    Temp[i] = 0.00;
  }
  //---------------------------------------- 

  // Calls the Update_OLED_Display() subroutine.
  Update_OLED_Display();

  delay(1000);

  // Calls the Rst_LORA() subroutine.
  Rst_LORA();

  // Calls the Getting_data_for_the_first_time() subroutine.
  Getting_data_for_the_first_time();
}
//________________________________________________________________________________ 

//________________________________________________________________________________ VOID LOOP
void loop() {
  // put your main code here, to run repeatedly:

  //---------------------------------------- Button condition to change the LED state on Slave 1.
  if (Button(1) == 1) Trig_Button_1_State = true;
  if (Button(1) == 0 && Trig_Button_1_State == true) {
    LED_1_State_Slave_1 = !LED_1_State_Slave_1;
    Trig_Button_1_State = false;
  }
  //----------------------------------------

  //---------------------------------------- Button condition to change the LED state on Slave 2.
  if (Button(2) == 1) Trig_Button_2_State = true;
  if (Button(2) == 0 && Trig_Button_2_State == true) {
    LED_1_State_Slave_2 = !LED_1_State_Slave_2;
    Trig_Button_2_State = false;
  }
  //----------------------------------------

  //---------------------------------------- Millis/Timer to send messages to slaves every 1 second (see interval_SendMSG variable).
  //  Messages are sent every one second is alternately.
  //  > Master sends message to Slave 1, delay 1 second.
  //  > Master sends message to Slave 2, delay 1 second.
  
  unsigned long currentMillis_SendMSG = millis();
  
  if (currentMillis_SendMSG - previousMillis_SendMSG >= interval_SendMSG) {
    previousMillis_SendMSG = currentMillis_SendMSG;

    //::::::::::::::::: The condition to update the OLED LCD if there is no incoming message from all slaves.
    Count_OLED_refresh_when_no_data_comes_in++;
    if (Count_OLED_refresh_when_no_data_comes_in > 5) {
      Count_OLED_refresh_when_no_data_comes_in = 0;
      Processing_incoming_data();
    }
    //::::::::::::::::: 

    //::::::::::::::::: Count the slaves.
    Slv++;
    if (Slv > 2) Slv = 1;
    //::::::::::::::::: 

    //::::::::::::::::: The condition for setting the variable used to change the state of the LEDs on the slaves.
    // "t" to turn on the LED.
    // "f" to turn off the LED.

    String send_LED_1_State_Slave_1 = "";
    String send_LED_1_State_Slave_2 = "";
    
    if (LED_1_State_Slave_1 == true) send_LED_1_State_Slave_1 = "t";
    if (LED_1_State_Slave_1 == false) send_LED_1_State_Slave_1 = "f";
    if (LED_1_State_Slave_2 == true) send_LED_1_State_Slave_2 = "t";
    if (LED_1_State_Slave_2 == false) send_LED_1_State_Slave_2 = "f";
    //::::::::::::::::: 

    // Enter the values of the send_LED_1_State_Slave_1 and send_LED_1_State_Slave_2 variables to the Message variable.
    Message = send_LED_1_State_Slave_1 + "," + send_LED_1_State_Slave_2;

    //::::::::::::::::: Condition for sending message / command data to Slave 1 (ESP32 Slave 1).
    if (Slv == 1) {
      Serial.println();
      Serial.println("Tr to  : 0x" + String(Destination_ESP32_Slave_1, HEX));
      Serial.println("Message: " + Message);
      Humd[0] = 0;
      Temp[0] = 0.00;
      sendMessage(Message, Destination_ESP32_Slave_1);
    }
    //::::::::::::::::: 
    
    //::::::::::::::::: Condition for sending message / command data to Slave 2 (ESP32 Slave 2).
    if (Slv == 2) {
      Serial.println();
      Serial.println("Tr to  : 0x" + String(Destination_ESP32_Slave_2, HEX));
      Serial.println("Message: " + Message);
      Humd[1] = 0;
      Temp[1] = 0.00;
      sendMessage(Message, Destination_ESP32_Slave_2);
    }
    //::::::::::::::::: 
  }
  //---------------------------------------- 

  //---------------------------------------- Millis/Timer to reset Lora Ra-02.
  //  - Lora Ra-02 reset is required for long term use.
  //  - That means the Lora Ra-02 is on and working for a long time.
  //  - From my experience when using Lora Ra-02 for a long time, there are times when Lora Ra-02 seems to "freeze" or an error, 
  //    so it can't send and receive messages. It doesn't happen often, but it does happen sometimes. 
  //    So I added a method to reset Lora Ra-02 to solve that problem. As a result, the method was successful in solving the problem.
  //  - This method of resetting the Lora Ra-02 works by checking whether there are incoming messages, 
  //    if no messages are received for approximately 30 seconds, then the Lora Ra-02 is considered to be experiencing "freezing" or error, so a reset is carried out.

  unsigned long currentMillis_RestartLORA = millis();
  
  if (currentMillis_RestartLORA - previousMillis_RestartLORA >= interval_RestartLORA) {
    previousMillis_RestartLORA = currentMillis_RestartLORA;

    Count_to_Rst_LORA++;
    if (Count_to_Rst_LORA > 30) {
      LoRa.end();
      Rst_LORA();
    }
  }
  //----------------------------------------

  //----------------------------------------parse for a packet, and call onReceive with the result:
  onReceive(LoRa.parsePacket());
  //----------------------------------------
}
//________________________________________________________________________________ 
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<



#################################################################################################
#################################################################################################
#################################################################################################
################################################################################################# 



//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 06 ESP32 Slave (1 or 2) DHT11 LED Lora Ra-02 Final
// More detailed information about this project, please look at the program code for the Master.

//---------------------------------------- Include Library.
#include <SPI.h>
#include <LoRa.h>
#include "DHT.h"
//---------------------------------------- 

//---------------------------------------- Defines the DHT11 Pin and the DHT type.
#define DHTPIN      15
#define DHTTYPE     DHT11
//---------------------------------------- 

//---------------------------------------- Defines LED Pins.
#define LED_1_Pin   27
//---------------------------------------- 

//---------------------------------------- LoRa Pin / GPIO configuration.
#define ss 5
#define rst 14
#define dio0 2
//----------------------------------------

// Initializes the DHT sensor (DHT11).
DHT dht11(DHTPIN, DHTTYPE);

//----------------------------------------String variable for LoRa
String Incoming = "";
String Message = "";
String CMD_LED_1_State = "";
//----------------------------------------

//---------------------------------------- LoRa data transmission configuration.
////////////////////////////////////////////////////////////////////////////
// PLEASE UNCOMMENT AND SELECT ONE OF THE "LocalAddress" VARIABLES BELOW. //
////////////////////////////////////////////////////////////////////////////

//byte LocalAddress = 0x02;       //--> address of this device (Slave 1).
//byte LocalAddress = 0x03;       //--> address of this device (Slave 2).

byte Destination_Master = 0x01; //--> destination to send to Master (ESP32).
//----------------------------------------

//---------------------------------------- Variable declarations for temperature and humidity values.
int h;
float t;
//---------------------------------------- 

//---------------------------------------- Variable declaration for Millis/Timer.
unsigned long previousMillis_UpdateDHT11 = 0;
const long interval_UpdateDHT11 = 2000;

unsigned long previousMillis_RestartLORA = 0;
const long interval_RestartLORA = 1000;
//---------------------------------------- 

// Declaration of variable as counter to restart Lora Ra-02.
byte Count_to_Rst_LORA = 0;

//________________________________________________________________________________ Subroutines for sending data (LoRa Ra-02).
void sendMessage(String Outgoing, byte Destination) {
  LoRa.beginPacket();             //--> start packet
  LoRa.write(Destination);        //--> add destination address
  LoRa.write(LocalAddress);       //--> add sender address
  LoRa.write(Outgoing.length());  //--> add payload length
  LoRa.print(Outgoing);           //--> add payload
  LoRa.endPacket();               //--> finish packet and send it
}
//________________________________________________________________________________ 

//________________________________________________________________________________ Subroutines for receiving data (LoRa Ra-02).
void onReceive(int packetSize) {
  if (packetSize == 0) return;          // if there's no packet, return

  //---------------------------------------- read packet header bytes:
  int recipient = LoRa.read();        //--> recipient address
  byte sender = LoRa.read();          //--> sender address
  byte incomingLength = LoRa.read();  //--> incoming msg length
  //---------------------------------------- 

  //---------------------------------------- Condition that is executed if message is not from Master.
  if (sender != Destination_Master) {
    Serial.println();
    Serial.println("i"); //--> "i" = Not from Master, Ignore.
    //Serial.println("Not from Master, Ignore");

    // Resets the value of the Count_to_Rst_LORA variable.
    Count_to_Rst_LORA = 0;
    return; //--> skip rest of function
  }
  //---------------------------------------- 

  // Clears Incoming variable data.
  Incoming = "";

  //---------------------------------------- Get all incoming data.
  while (LoRa.available()) {
    Incoming += (char)LoRa.read();
  }
  //---------------------------------------- 

  // Resets the value of the Count_to_Rst_LORA variable.
  Count_to_Rst_LORA = 0;

  //---------------------------------------- Check length for error.
  if (incomingLength != Incoming.length()) {
    Serial.println();
    Serial.println("er"); //--> "er" = error: message length does not match length.
    //Serial.println("error: message length does not match length");
    return; //--> skip rest of function
  }
  //---------------------------------------- 

  //---------------------------------------- Checks whether the incoming data or message for this device.
  if (recipient != LocalAddress) {
    Serial.println();
    Serial.println("!");  //--> "!" = This message is not for me.
    //Serial.println("This message is not for me.");

    // Calls the Processing_incoming_data_for_LEDs() subroutine.
    Processing_incoming_data_for_LEDs();
    return; //--> skip rest of function
  } else {
    // if message is for this device, or broadcast, print details:
    Serial.println();
    Serial.println("Rc from: 0x" + String(sender, HEX));
    Serial.println("Message: " + Incoming);

    // Calls the Processing_incoming_data() subroutine.
    Processing_incoming_data();
  }
}
//________________________________________________________________________________ 

//________________________________________________________________________________ Subroutine to process the data to be sent, after that it sends a message to the Master.
void Processing_incoming_data() {
  // Calls the Processing_incoming_data_for_LEDs() subroutine.
  Processing_incoming_data_for_LEDs();

  // Get the last state of the LED.
  byte LED_1_State = digitalRead(LED_1_Pin);

  // Fill in the "Message" variable with the value of humidity, temperature and the last state of the LED.
  Message = String(h) + "," + String(t) + "," + String(LED_1_State);
  
  Serial.println();
  Serial.println("Tr to  : 0x" + String(Destination_Master, HEX));
  Serial.println("Message: " + Message);

  // Send a message to Master.
  sendMessage(Message, Destination_Master);
}
//________________________________________________________________________________ 

//________________________________________________________________________________ Subroutines for processing data from incoming messages.
//  - The data contents of the message sent by the Master are: f,f ("t" to turn on the LED and "f" to turn off the LED.)
//
//  - The first "f" is to control the LED on Slave 1. If using the "GetValue" subroutine, this first "f" is in position 0.
//
//  - The second "f" is to control the LEDs on Slave 2. If using the "GetValue" subroutine, this second "f" is in position 1.
//
//  - If this code is for Slave 1, the first "f" is taken. So : CMD_LED_1_State = GetValue(Incoming, ',', 0);
//
//  - If this code is for Slave 2, the first "f" is taken. So : CMD_LED_1_State = GetValue(Incoming, ',', 1);

void Processing_incoming_data_for_LEDs() {
/////////////////////////////////////////////////////////////////////////////////////////
// PLEASE UNCOMMENT THE LINE OF CODE BELOW IF THIS CODE OR THIS DEVICE IS FOR SLAVE 1. //
/////////////////////////////////////////////////////////////////////////////////////////

//  CMD_LED_1_State = GetValue(Incoming, ',', 0);
//
//  if (CMD_LED_1_State == "t") digitalWrite(LED_1_Pin, HIGH);
//  if (CMD_LED_1_State == "f") digitalWrite(LED_1_Pin, LOW);


/////////////////////////////////////////////////////////////////////////////////////////
// PLEASE UNCOMMENT THE LINE OF CODE BELOW IF THIS CODE OR THIS DEVICE IS FOR SLAVE 2. //
/////////////////////////////////////////////////////////////////////////////////////////

//  CMD_LED_1_State = GetValue(Incoming, ',', 1);
//
//  if (CMD_LED_1_State == "t") digitalWrite(LED_1_Pin, HIGH);
//  if (CMD_LED_1_State == "f") digitalWrite(LED_1_Pin, LOW);
}
//________________________________________________________________________________ 

//________________________________________________________________________________ String function to process the data received
// I got this from : https://www.electroniclinic.com/reyax-lora-based-multiple-sensors-monitoring-using-arduino/
String GetValue(String data, char separator, int index) {
  int found = 0;
  int strIndex[] = { 0, -1 };
  int maxIndex = data.length() - 1;
  
  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}
//________________________________________________________________________________ 

//________________________________________________________________________________ Subroutine to reset Lora Ra-02.
void Rst_LORA() {
  LoRa.setPins(ss, rst, dio0);

  Serial.println();
  Serial.println(F("Restart LoRa..."));
  Serial.println(F("Start LoRa init..."));
  if (!LoRa.begin(433E6)) {             // initialize ratio at 915 or 433 MHz
    Serial.println(F("LoRa init failed. Check your connections."));
    while (true);                       // if failed, do nothing
  }
  Serial.println(F("LoRa init succeeded."));

  // Resets the value of the Count_to_Rst_LORA variable.
  Count_to_Rst_LORA = 0;
}
//________________________________________________________________________________ 

//________________________________________________________________________________ VOID SETUP
void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);

  pinMode(LED_1_Pin, OUTPUT);

  // Calls the Rst_LORA() subroutine.
  Rst_LORA();
  
  dht11.begin();
}
//________________________________________________________________________________ 

//________________________________________________________________________________ VOID LOOP
void loop() {
  // put your main code here, to run repeatedly:

  //---------------------------------------- Millis / Timer to update the temperature and humidity values ​​from the DHT11 sensor every 2 seconds (see the variable interval_UpdateDHT11).
  unsigned long currentMillis_UpdateDHT11 = millis();
  
  if (currentMillis_UpdateDHT11 - previousMillis_UpdateDHT11 >= interval_UpdateDHT11) {
    previousMillis_UpdateDHT11 = currentMillis_UpdateDHT11;

    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    h = dht11.readHumidity();
    // Read temperature as Celsius (the default)
    t = dht11.readTemperature();
    // Read temperature as Fahrenheit (isFahrenheit = true)
    //float f = dht.readTemperature(true);
  
    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t)) {
      Serial.println(F("Failed to read from DHT sensor!"));
      return;
    }
  }
  //---------------------------------------- 

  //---------------------------------------- Millis/Timer to reset Lora Ra-02.
  // Please see the Master program code for more detailed information about the Lora reset method.
  
  unsigned long currentMillis_RestartLORA = millis();
  
  if (currentMillis_RestartLORA - previousMillis_RestartLORA >= interval_RestartLORA) {
    previousMillis_RestartLORA = currentMillis_RestartLORA;

    Count_to_Rst_LORA++;
    if (Count_to_Rst_LORA > 30) {
      LoRa.end();
      Rst_LORA();
    }
  }
  //---------------------------------------- 

  //---------------------------------------- parse for a packet, and call onReceive with the result:
  onReceive(LoRa.parsePacket());
  //----------------------------------------
}
//________________________________________________________________________________ 
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<