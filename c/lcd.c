#include <wiringPi.h>
#include <pcf8574.h>
#include <lcd.h>
#include <stdio.h>

#define LCD_BASE_ADDR 100
#define LCD_RS_PIN LCD_BASE_ADDR + 0
#define LCD_RW_PIN LCD_BASE_ADDR + 1
#define LCD_EN_PIN LCD_BASE_ADDR + 2
#define LCD_BL_PIN LCD_BASE_ADDR + 3
#define LCD_D4_PIN LCD_BASE_ADDR + 4
#define LCD_D5_PIN LCD_BASE_ADDR + 5
#define LCD_D6_PIN LCD_BASE_ADDR + 6
#define LCD_D7_PIN LCD_BASE_ADDR + 7

void lcdInit() {
    pcf8574Setup(LCD_RS_PIN, 0x27);
    pinMode(LCD_RW_PIN, OUTPUT);
    digitalWrite(LCD_RW_PIN, LOW);
    pinMode(LCD_BL_PIN, OUTPUT);
    digitalWrite(LCD_BL_PIN, HIGH);
}

void lcdWrite(char* str) {
    int fd = lcdInit(2, 16, 4, LCD_RS_PIN, LCD_EN_PIN, LCD_D4_PIN, LCD_D5_PIN, LCD_D6_PIN, LCD_D7_PIN, 0, 0, 0, 0);
    lcdClear(fd); // 화면 지우기
    delay(100);

    lcdPosition(fd, 2, 0); // 윗줄 2번째 칸
    lcdPuts(fd, str); // 문자열 출력

    lcdPosition(fd, 2, 1); // 아랫줄 2번째 칸
    lcdPuts(fd, str); // 문자열 출력
}

void lcdClear() {
    int fd = lcdInit(2, 16, 4, LCD_RS_PIN, LCD_EN_PIN, LCD_D4_PIN, LCD_D5_PIN, LCD_D6_PIN, LCD_D7_PIN, 0, 0, 0, 0);
    lcdClear(fd); // 화면 지우기
    delay(100);
}

void lcdWriteOver(char* str) {
    int fd = lcdInit(2, 16, 4, LCD_RS_PIN, LCD_EN_PIN, LCD_D4_PIN, LCD_D5_PIN, LCD_D6_PIN, LCD_D7_PIN, 0, 0, 0, 0);
    lcdClear(fd); // 화면 지우기
    delay(100);

    lcdPosition(fd, 2, 0); // 윗줄 2번째 칸
    lcdPuts(fd, str); // 문자열 출력
}

void lcdWriteUnder(char* str) {
    int fd = lcdInit(2, 16, 4, LCD_RS_PIN, LCD_EN_PIN, LCD_D4_PIN, LCD_D5_PIN, LCD_D6_PIN, LCD_D7_PIN, 0, 0, 0, 0);
    lcdPosition(fd, 2, 1); // 아랫줄 2번째 칸
    lcdPuts(fd, str); // 문자열 출력
}