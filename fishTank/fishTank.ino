#include <Servo.h>

Servo motor_large;
Servo motor_1;
Servo motor_2;
Servo motor_3;

void setup() {
  motor_large.attach(3);
  motor_1.attach(5);
  motor_2.attach(6);
  motor_3.attach(9);
  pinMode(A5, INPUT);
  pinMode(10, OUTPUT);
  motor_1.write(100);
  motor_2.write(100);
  motor_3.write(180);
  motor_large.write(30);
  Serial.begin(9600);
}

void loop() {
  int photoVal = 0;
  photoVal = analogRead(A5);
  Serial.print("photoVal = ");
  Serial.println(photoVal);
  motor_1.write(100);
  motor_2.write(130);
  motor_3.write(180);
  delay(1000);
  motor_1.write(135);
  motor_2.write(165);
  if (photoVal <= 400) {
    motor_large.write(135);
    digitalWrite(10, HIGH);
  }  
  else 
  {
    digitalWrite(10, LOW);
    motor_3.write(140);
    motor_large.write(30);
  }
  delay(1000);
}
