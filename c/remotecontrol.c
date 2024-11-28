#include "irpigpio.h"
#include <stdio.h>
#include <wiringPi.h>

#define IR_PIN 18
#define LED 27
#define BUTTON_0 951308750
#define BUTTON_1 271208262

int initRemote(){
    // if (gpioInitialise() < 0 || wiringPiSetup() < 0) {
    //     return -1;
    // }
    gpioSetMode(IR_PIN, PI_INPUT);
    gpioSetWatchdog(IR_PIN, 5);
    gpioSetAlertFunc(IR_PIN, alert);
}


'''
int main() {
    long value;
    while(1) {
        value = readIR();
        switch(value){
            case BUTTON_0:
                printf("Button 0 pressed, LED On\n");
                break;
            case BUTTON_1:
                printf("Button 1 pressed, LED Off\n");
                break;
    }
    gpioTerminate();
}
}
'''
