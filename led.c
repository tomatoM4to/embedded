#include <stdio.h>
#include <wiringPi.h>

void wiringPiInit(void) {
    pinMode(27, OUTPUT); // led red
    pinMode(28, OUTPUT); // led green
    pinMode(29, OUTPUT); // led blue

    pinMode(30, OUTPUT); // simple buzz
}

void redLedOnOff(int d = 0) {
    digitalWrite(27, HIGH);
}

void redLedOff(void) {
    digitalWrite(27, LOW);
}

void greenLedOn(void) {
    digitalWrite(28, HIGH);
}

void greenLedOff(void) {
    digitalWrite(28, LOW);
}

void blueLedOn(void) {
    digitalWrite(29, HIGH);
}

void blueLedOff(void) {
    digitalWrite(29, LOW);
}

void buzzOn(void) {
    digitalWrite(30, HIGH);
}

void buzzOff(void) {
    digitalWrite(30, LOW);
}

void camera() {
    char cmd[] = "sudo libcamera-jpeg --width 800 --height 800 -t 1 -o test.jpeg";
    system(cmd);
    printf("Take a picture \n");
}


int main(void) {
    if (wiringPiSetup() == -1) {
        return 1;
    }


    wiringPiInit();

    return 0;
}