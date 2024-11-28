#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <wiringPi.h>
#include <softPwm.h>
#include <wiringSerial.h>

//- 핀 정의 ---------------------------------------------------------------------
#define DCMotorA    6       // DC 모터 제어 핀 A
#define DCMotorB    31      // DC 모터 제어 핀 B
#define SERVO_PIN   26      // 서보 모터 제어 핀

//- 전역 변수 및 상수 -------------------------------------------------------------
char rx_buf[BUFSIZ] = "";
int g_hBT;
int current_speed = 50;     // 현재 속도 (0-100)

//- 함수 선언 ---------------------------------------------------------------------
void hw_init(void);
void InitBT(void);
void parserCmd(void);
void dcmotorInit(void);
void servoInit(void);
void dcmotorRight(int speed);
void dcmotorLeft(int speed);
void dcmotorStop(void);
void servoSetAngle(int angle);
void servoStop(void);

int main() {
    unsigned int nNextTime;

    InitBT();
    hw_init();

    nNextTime = millis();

    while(1) {
        if(millis() > nNextTime + 250) {
            int i = 0;
            memset(rx_buf, 0, sizeof(rx_buf));
            while(serialDataAvail(g_hBT) > 0) {
                rx_buf[i] = serialGetchar(g_hBT);
                if(rx_buf[i] == '#') {
                    if(rx_buf[0] == '@') parserCmd();
                    break;
                }
                i++;
            }
            nNextTime = millis();
        }
    }
    return 0;
}

void hw_init(void) {
    if(wiringPiSetup() == -1) {
        fprintf(stderr, "wiringPiSetup Failed! Error: %s\n", strerror(errno));
        return;
    }

    dcmotorInit();
    servoInit();
}

void InitBT(void) {
    g_hBT = serialOpen("/dev/rfcomm0", 115200);
    printf("Bluetooth init\n");
}

void parserCmd(void) {
    int index = 1;

    // DC 모터 제어 명령
    if(!strncmp(&rx_buf[1], "DCM", 3)) {
        index += 3;
        if(rx_buf[index] == ',') {
            index++;
            if(rx_buf[index] == 'R') {  // 시계 방향
                index += 2;
                current_speed = atoi(&rx_buf[index]);
                dcmotorRight(current_speed);
                printf("DC Motor Right: speed=%d\n", current_speed);
            }
            else if(rx_buf[index] == 'L') {  // 반시계 방향
                index += 2;
                current_speed = atoi(&rx_buf[index]);
                dcmotorLeft(current_speed);
                printf("DC Motor Left: speed=%d\n", current_speed);
            }
            else if(rx_buf[index] == 'S') {  // 정지
                dcmotorStop();
                printf("DC Motor Stop\n");
            }
        }
    }
    // 서보 모터 제어 명령
    else if(!strncmp(&rx_buf[1], "SRV", 3)) {
        index += 3;
        if(rx_buf[index] == ',') {
            index++;
            int angle = atoi(&rx_buf[index]);
            if(angle >= 0 && angle <= 180) {
                servoSetAngle(angle);
                printf("Servo Angle: %d\n", angle);
            }
        }
    }
}

//- DC 모터 관련 함수 -----------------------------------------------------------
void dcmotorInit(void) {
    pinMode(DCMotorA, OUTPUT);
    pinMode(DCMotorB, OUTPUT);
    dcmotorStop();
}

void dcmotorRight(int speed) {
    digitalWrite(DCMotorA, HIGH);
    digitalWrite(DCMotorB, LOW);
    // PWM으로 속도 제어할 경우 여기에 PWM 코드 추가
}

void dcmotorLeft(int speed) {
    digitalWrite(DCMotorA, LOW);
    digitalWrite(DCMotorB, HIGH);
    // PWM으로 속도 제어할 경우 여기에 PWM 코드 추가
}

void dcmotorStop(void) {
    digitalWrite(DCMotorA, LOW);
    digitalWrite(DCMotorB, LOW);
}

//- 서보 모터 관련 함수 ---------------------------------------------------------
void servoInit(void) {
    pinMode(SERVO_PIN, SOFT_PWM_OUTPUT);
    softPwmCreate(SERVO_PIN, 0, 200);
    servoSetAngle(90);  // 초기 위치는 중앙
}

void servoSetAngle(int angle) {
    softPwmWrite(SERVO_PIN, angle);
}

void servoStop(void) {
    softPwmStop(SERVO_PIN);
}