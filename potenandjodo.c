#include <stdio.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <errno.h>
#include <string.h>

// 금고문제를 해결하기 위한 코드

// SPI & ADC 설정값 정의
#define CHAN_CONFIG_SINGLE 8      // 싱글 채널 설정 값
#define SPI_SPEED 1000000        // 1MHz
#define SPI_CE_CHANNEL 1         // SPI와 연결된 CE 채널
#define SPI_ADC_POTEN_CHANNEL 1  // Potentiometer 연결 채널
#define CS_MCP3208 11            // MCP3208 연결 핀
#define SPI_ADC_JODO_CHANNEL 0   // 조도 센서 연결 채널

int spiSetup(void){
    if (wiringPiSPISetup(SPI_CE_CHANNEL, SPI_SPEED) == -1 ) {
        fprintf(stderr, "wiringPiSPISetup Failed ! ERROR : %s\n", strerror(errno));
    return 1;
}
}

int readAnalogData(int adcChannel) {
   int adcValue = 0;
   unsigned char buffer[3] = {0};

   // MCP3008의 채널 범위 체크 (0-7)
   if(adcChannel < 0 || adcChannel > 7) {
       return -1;
   }

   // SPI 통신 버퍼 설정
   buffer[0] = 0x01;
   buffer[1] = (CHAN_CONFIG_SINGLE + adcChannel) << 4;

   // SPI 통신 시작 (칩 활성화)
   digitalWrite(CS_MCP3208, LOW);

   // ADC 값 읽기
   wiringPiSPIDataRW(SPI_CE_CHANNEL, buffer, 3);

   // 10비트 데이터 추출
   adcValue = ((buffer[1] & 3) << 8) + buffer[2];

   // SPI 통신 종료 (칩 비활성화)
   digitalWrite(CS_MCP3208, HIGH);

   return adcValue;
}

void PotentiometerAndJodoInit() {
    // SPI 설정
    spiSetup();

    // CS 핀 설정
    pinMode(CS_MCP3208, OUTPUT);
}

// example
// adcValue = readAnalogData(SPI_ADC_POTEN_CHANNEL); delay(500); 센서 값 가져오기
// adcValue = readAnalogData(SPI_ADC_JODO_CHANNEL); delay(500); 센서 값 가져오기
