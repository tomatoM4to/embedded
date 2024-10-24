#include <wiringPi.h>
#include <softPwm.h>
#include <stdio.h>

// DC 모터 핀 정의
#define DCMotorA 6
#define DCMotorB 31

void dcmotorInit() {
    pinMode(DCMotorA, OUTPUT);
    pinMode(DCMotorB, OUTPUT);
}

void dcmotorRight() {
    digitalWrite(DCMotorA, HIGH);
    digitalWrite(DCMotorB, LOW);
    // delay(1000); 1초동안 회전
}

void dcmotorLeft() {
    digitalWrite(DCMotorA, LOW);
    digitalWrite(DCMotorB, HIGH);
    // delay(1000); 1초동안 회전
}

void dcmotorStop() {
    digitalWrite(DCMotorA, LOW);
    digitalWrite(DCMotorB, LOW);
}