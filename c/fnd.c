#include <stdio.h>
#include <wiringPi.h>
#include "TM1637.h"

#define CLK_PIN 9
#define DIO_PIN 8

void fndInit() {
    pinMode(CLK_PIN, OUTPUT);
    pinMode(DIO_PIN, OUTPUT);

    TM1637_init(CLK_PIN, DIO_PIN);
    TM1637_set( BRIGHTEST, 0x40, 0xc0 );
}

void fndDisplay(char* str) {
    for (int i = 0; i < 4 && str[i] != '\0'; i++) {
        TM1637_display(i, str[i] - '0');
    }
}

void fndClear() {
    TM1637_clear();
}

void fndOff() {
    for(int i = 0; i < 4; i++) {
        TM1637_display(i, 0);  // 0을 표시하거나 빈 값을 표시
    }
}