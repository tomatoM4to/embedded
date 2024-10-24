#include <stdio.h>
#include <wiringPi.h>

#define ULTRASONIC_TRIG 4
#define ULTRASONIC_ECHO 5

void ultrasonicInit(void) {
    pinMode(ULTRASONIC_TRIG, OUTPUT);
    pinMode(ULTRASONIC_ECHO, INPUT);
}

void ultrasonicOn(void) {
    digitalWrite(ULTRASONIC_TRIG, LOW);
    delay(500);
    digitalWrite(ULTRASONIC_TRIG, HIGH);
    delayMicroseconds(10);
    digitalWrite(ULTRASONIC_TRIG, LOW);
}

float ultraSonicGetDistance(void) {
    int start_time = 0, end_time = 0;
    float duration = 0, distance = 0;

    // 초음파 발사 시작 시간 측정
    while(digitalRead(ULTRASONIC_ECHO) == LOW)
        start_time = micros();

    // 초음파 반사되어 돌아온 시간 측정
    while(digitalRead(ULTRASONIC_ECHO) == HIGH)
        end_time = micros();

    // 거리 계산
    duration = (float)(end_time - start_time);
    duration = duration/1000000/2;
    distance = (340 * duration) * 100;

    return distance;
}

int ultroSonicExample(void) {
    if(wiringPiSetup() == -1) {
        printf("Setup Fail ~!!");
        return -1;
    }

    ultrasonicInit();

    while(1) {
        ultrasonicOn();
        float distance = ultraSonicGetDistance();
        printf("Distance : %3.0f cm\n", distance);
    }

    return 0;
}