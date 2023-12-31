#include <Servo.h>

Servo myservo;

unsigned int temp1, temp2;

signed int temp3;

unsigned long startMillis = 0;
const long interval = 10;

unsigned long startMillis1 = 0;
const long interval1 = 1000;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  myservo.attach(5);
  myservo.write(90);
}

void loop() {
  // put your main code here, to run repeatedly:
  unsigned long currentMillis = millis();
  temp1 = analogRead(A1);
  temp2 = analogRead(A2);

  if (currentMillis - startMillis >= 50) {
    startMillis = currentMillis;
    temp3 = temp2 - temp1;
  }

  if(currentMillis - startMillis1 >= interval1){
    startMillis1 = currentMillis;
    if(temp3 <= -50){
      myservo.write(135);
    }

    if(temp3 > -50 && temp3 <= -20){
      myservo.write(115);
    }

    if(temp3 > -10 && temp3 <= 10){
      myservo.write(90);
    }

    if(temp3 > 20 && temp3 <= 50){
      myservo.write(85);
    }

    if(temp3 >= 50){
      myservo.write(45);
    }
  }
  // Serial.println(temp1);
  // Serial.println(temp2);
  Serial.println(temp3);

  // delay(200);
}
