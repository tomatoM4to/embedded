#include <wiringPi.h>
#include <pcf8574.h>
#include <lcd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <wiringPiSPI.h>
#include <wiringSerial.h>

// SPI ADC 관련 정의
#define CHAN_CONFIG_SINGLE 8
#define SPI_SPEED 1000000
#define SPI_CE_CHANNEL 1
#define SPI_ADC_LIGHT_CHANNEL 0
#define SPI_ADC_POTEN_CHANNEL 1
#define CS_MCP3208 11
#define BUZ_PIN 30

// LCD 관련 정의
#define LCD_BASE_ADDR 100
#define LCD_RS_PIN LCD_BASE_ADDR + 0
#define LCD_RW_PIN LCD_BASE_ADDR + 1
#define LCD_EN_PIN LCD_BASE_ADDR + 2
#define LCD_BL_PIN LCD_BASE_ADDR + 3
#define LCD_D4_PIN LCD_BASE_ADDR + 4
#define LCD_D5_PIN LCD_BASE_ADDR + 5
#define LCD_D6_PIN LCD_BASE_ADDR + 6
#define LCD_D7_PIN LCD_BASE_ADDR + 7

// LED 관련
enum { LED_RED, LED_GREEN, LED_BLUE};
static unsigned char LED[] = {27, 28, 29};

// 전역 변수
int potential, illum;
int bBuzzerRun = 0;
char rx_buf[BUFSIZ] = "";
int g_hBT;
int lcd_handle; // LCD 파일 디스크립터

// 함수 선언
void hw_init(void);
void InitBT(void);
void parserCmd(void);
void sendSensorData(void);
void LedOn(int type, int bOn);
void BuzzerOn(int bOn);
int readAnalogData(int adcChannel);
void lcdInit(void);
void lcdWrite(char* str);
void lcdClear(void);
void lcdWriteOver(char* str);
void lcdWriteUnder(char* str);

int main() {
    unsigned int nNextTime, nSensorTime;

    // 초기화
    wiringPiSetup();
    InitBT();
    hw_init();
    lcdInit();

    nNextTime = millis();
    nSensorTime = millis();

    while(1) {
        if(millis() > nNextTime + 250) {
            int i=0;
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

        if(millis() > nSensorTime + 1000) {
            potential = readAnalogData(SPI_ADC_POTEN_CHANNEL);
            illum = readAnalogData(SPI_ADC_LIGHT_CHANNEL);
            printf("Send Sensor Data\n");
            sendSensorData();
            nSensorTime = millis();
        }
    }
    return 0;
}

void hw_init(void) {
    if(wiringPiSPISetup(SPI_CE_CHANNEL, SPI_SPEED) == -1) {
        fprintf(stderr, "wiringPiSPISetup Failed! ERROR: %s\n", strerror(errno));
        return;
    }

    int i;
    for(i=0; i<sizeof(LED); i++) {
        pinMode(LED[i], OUTPUT);
        digitalWrite(LED[i], LOW);
    }

    pinMode(BUZ_PIN, OUTPUT);
}

void lcdInit() {
    pcf8574Setup(LCD_RS_PIN, 0x27);
    pinMode(LCD_RW_PIN, OUTPUT);
    digitalWrite(LCD_RW_PIN, LOW);
    pinMode(LCD_BL_PIN, OUTPUT);
    digitalWrite(LCD_BL_PIN, HIGH);

    lcd_handle = lcdInit(2, 16, 4, LCD_RS_PIN, LCD_EN_PIN,
                        LCD_D4_PIN, LCD_D5_PIN, LCD_D6_PIN, LCD_D7_PIN,
                        0, 0, 0, 0);
    if (lcd_handle < 0) {
        fprintf(stderr, "lcdInit failed!\n");
        return;
    }
    lcdClear();
}

void lcdWrite(char* str) {
    lcdClear();
    delay(100);

    lcdPosition(lcd_handle, 2, 0);
    lcdPuts(lcd_handle, str);

    lcdPosition(lcd_handle, 2, 1);
    lcdPuts(lcd_handle, str);
}

void lcdClear() {
    lcdClear(lcd_handle);
    delay(100);
}

void lcdWriteOver(char* str) {
    lcdClear();
    delay(100);

    lcdPosition(lcd_handle, 2, 0);
    lcdPuts(lcd_handle, str);
}

void lcdWriteUnder(char* str) {
    lcdPosition(lcd_handle, 2, 1);
    lcdPuts(lcd_handle, str);
}

void parserCmd() {
    int index = 1;
    char lcd_text[17] = {0};  // 16자 + null 종료문자

    if(!strncmp(&rx_buf[1], "LCD", 3)) {
        index += 3;
        if(rx_buf[index] == ',') {
            index++;
            int i = 0;
            while(rx_buf[index] != '#' && i < 16) {
                lcd_text[i++] = rx_buf[index++];
            }
            lcd_text[i] = '\0';
            lcdWriteOver(lcd_text);
            printf("LCD Display: %s\n", lcd_text);
        }
    } else if(!strncmp(&rx_buf[1], "BLT", 3)) {
        index += 3;
        if(rx_buf[index] == ',') {
            index++;
            if(rx_buf[index] == '0') {
                digitalWrite(LCD_BL_PIN, LOW);
                printf("LCD Backlight OFF\n");
            } else if(rx_buf[index] == '1') {
                digitalWrite(LCD_BL_PIN, HIGH);
                printf("LCD Backlight ON\n");
            }
        }
    } else if(!strncmp(&rx_buf[1], "CLR", 3)) {
        lcdClear();
        printf("LCD Cleared\n");
    } else if(!strncmp(&rx_buf[1], "BUZ", 3)) {
        index += 3;
        if(rx_buf[index] == ',') {
            int bOn;
            index++;
            if(rx_buf[index] == '0') {
                printf("Buzzer off\n");
                bOn = 0;
            } else if(rx_buf[index] == '1') {
                printf("Buzzer on\n");
                bOn = 1;
            }
            index++;
            if(rx_buf[index]== '#') {
                BuzzerOn(bOn);
                bBuzzerRun = bOn;
            }
        }
    } else if(!strncmp(&rx_buf[1], "RED", 3)) {
        // LED 제어 코드는 이전과 동일
        index += 3;
        if(rx_buf[index] == ',') {
            int bOn = 0;
            index++;
            if(rx_buf[index] == '0') {
                printf("Red LED off\n");
                bOn = 0;
            } else if(rx_buf[index] == '1') {
                printf("Red LED on\n");
                bOn = 1;
            }
            index++;
            if(rx_buf[index]== '#')
                LedOn(LED_RED, bOn);
        }
    } else if(!strncmp(&rx_buf[1], "GED", 3)) {
        index += 3;
        if(rx_buf[index] == ',') {
            int bOn = 0;
            index++;
            if(rx_buf[index] == '0') {
                printf("Green LED off\n");
                bOn = 0;
            } else if(rx_buf[index] == '1') {
                printf("Green LED on\n");
                bOn = 1;
            }
            index++;
            if(rx_buf[index]== '#')
                LedOn(LED_GREEN, bOn);
        }
    } else if(!strncmp(&rx_buf[1], "BED", 3)) {
        index += 3;
        if(rx_buf[index] == ',') {
            int bOn = 0;
            index++;
            if(rx_buf[index] == '0') {
                printf("Blue LED off\n");
                bOn = 0;
            } else if(rx_buf[index] == '1') {
                printf("Blue LED on\n");
                bOn = 1;
            }
            index++;
            if(rx_buf[index]== '#')
                LedOn(LED_BLUE, bOn);
        }
    } else {
        printf("Unknown command: %s\n", rx_buf);
    }
}

void InitBT() {
    g_hBT = serialOpen("/dev/rfcomm0", 115200);
    printf("Bluetooth init\n");
}

void sendSensorData() {
    serialPrintf(g_hBT, "@POT,%d#@ILL,%d#", potential, illum);
}

void LedOn(int type, int bOn) {
    digitalWrite(LED[type], bOn);
}

void BuzzerOn(int bOn) {
    if(bOn)
        digitalWrite(BUZ_PIN, HIGH);
    else
        digitalWrite(BUZ_PIN, LOW);
}

int readAnalogData(int adcChannel) {
    int adcValue = 0;
    unsigned char buffer[3] = {0};

    if(adcChannel < 0 || adcChannel > 7) return -1;

    buffer[0] = 0x01;
    buffer[1] = (CHAN_CONFIG_SINGLE + adcChannel) << 4;

    digitalWrite(CS_MCP3208, LOW);
    wiringPiSPIDataRW(SPI_CE_CHANNEL, buffer, 3);
    adcValue = ((buffer[1] & 3) << 8) + buffer[2];
    digitalWrite(CS_MCP3208, HIGH);

    return adcValue;
}