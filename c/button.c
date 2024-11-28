#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>

#define PUSH_SWITCH_R 0
#define PUSH_SWITCH_L 7

void buttonInit() {
    pinMode(PUSH_SWITCH_R, INPUT);
    pinMode(PUSH_SWITCH_L, INPUT);
}

// digitalRead(PUSH_SWITCH_R) == LOW : 버튼이 눌렸을 때
// digitalRead(PUSH_SWITCH_L) == LOW : 버튼이 눌렸을 때
