#ifndef DOOR_AUTOMATION_H
#define DOOR_AUTOMATION_H
#include<Servo.h>
Servo servoMotor;
int servoPin = 5; 
int servoTrans = 11;

void servoSetUp(){
     servoMotor.attach(servoPin);
     pinMode(servoTrans, OUTPUT);

     digitalWrite(servoTrans, HIGH);                   
     servoMotor.write(0);
     delay(800);
     digitalWrite(servoTrans, LOW);
     delay(300);
}

#endif
