#include <stdio.h>
#include <wiringPi.h>
#include <softPwm.h>

'''
gcc -o main \
    main.c \
    button.c \
    camera.c \
    dcmotor.c \
    fnd.c \
    lcd.c \
    led.c \
    potenandjodo.c \
    remotecontrol.c \
    servomoter.c \
    sound.c \
    stepmotor.c \
    tostring.c \
    ultrasonic.c \
    TM1637.c \
    irpigpio.c \
    -lwiringPi \
    -lwiringPiDev \
    -lpthread \
    -lpigpio \
    -I. \
    -fcommon
'''

// gcc -o main main.c button.c camera.c dcmotor.c fnd.c lcd.c led.c potenandjodo.c remotecontrol.c servomoter.c sound.c stepmotor.c tostring.c ultrasonic.c TM1637.c irpigpio.c -lwiringPi -lwiringPiDev -lpthread -lpigpio -I. -fcommon
void ledInit(void) {
    pinMode(27, SOFT_PWM_OUTPUT); // led red
    pinMode(28, SOFT_PWM_OUTPUT); // led green
    pinMode(29, SOFT_PWM_OUTPUT); // led blue

    softPwmCreate(27, 0, 255); // led red
    softPwmCreate(28, 0, 255); // led green
    softPwmCreate(29, 0, 255); // led blue
}

void redLedOn(int q) {
    softToneWrite(27, q);
}

void redLedOff() {
    softToneWrite(27, 0);
}

void greenLedOn(int w = 255) {
    softToneWrite(28, w);
}

void greenLedOff() {
    softToneWrite(28, 0);
}

void blueLedOn(int e = 255) {
    softToneWrite(29, e);
}

void blueLedOff() {
    softToneWrite(29, 0);
}

void allLedOff() {
    softToneWrite(27, 0);
    softToneWrite(28, 0);
    softToneWrite(29, 0);
}