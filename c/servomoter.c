#include <wiringPi.h>
#include <softPwm.h>
#include <stdio.h>

#define SERVO_PIN 26


void servoInit(void) {
    pinMode(SERVO_PIN, SOFT_PWM_OUTPUT);
    softPwmCreate(SERVO_PIN, 0, 200);
}

void servoSetAngle(int angle) {
    softPwmWrite(SERVO_PIN, angle);
}

void servoStop(void) {
    softPwmStop(SERVO_PIN);
}


int servoExample(void) {
    servoInit();
    while(1){
        //- 15도 방향 이동
        servoSetAngle(15);
        delay(500);
        //- 24도 방향 이동
        servoSetAngle(24);
        delay(500);
        //- 5도 방향 이동
        servoSetAngle(5);
        delay(500);
    }
    servoStop();
    delay(500);
    return 0;
}