#include <wiringPi.h>
#include <softTone.h>
#include <stdio.h>

#define BUZ_P_PIN 21

// 음계 정의 (도레미파솔라시도)
unsigned int melody[8] = {262, 294, 330, 349, 392, 440, 494, 523};

void buzzerInit(void) {
    pinMode(BUZ_P_PIN, SOFT_TONE_OUTPUT);
    pinMode(30, OUTPUT); // simple buzz
    softToneCreate(BUZ_P_PIN);
}

void buzzerOn(char v = 'q') {
    if (v == 'q') {
        digitalWrite(30, HIGH);
    }
    if (v == 'a') {
        softToneWrite(BUZ_P_PIN, 262);
    }
    if (v == 's') {
        softToneWrite(BUZ_P_PIN, 294);
    }
    if (v == 'd') {
        softToneWrite(BUZ_P_PIN, 330);
    }
    if (v == 'f') {
        softToneWrite(BUZ_P_PIN, 349);
    }
    if (v == 'g') {
        softToneWrite(BUZ_P_PIN, 392);
    }
    if (v == 'h') {
        softToneWrite(BUZ_P_PIN, 440);
    }
    if (v == 'j') {
        softToneWrite(BUZ_P_PIN, 494);
    }
    if (v == 'k') {
        softToneWrite(BUZ_P_PIN, 523);
    }
}

void buzzerStop(void) {
    softToneStop(BUZ_P_PIN);
    digitalWrite(30, LOW);
}