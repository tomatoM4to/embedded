#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <wiringPi.h>
#include <wiringSerial.h>
#include <wiringPiI2C.h>
#include <wiringPiSPI.h>

// SPI ADC
#define CHAN_CONFIG_SINGLE 8
#define SPI_SPEED 1000000
#define SPI_CE_CHANNEL 1
#define SPI_ADC_LIGHT_CHANNEL 0
#define SPI_ADC_POTEN_CHANNEL 1
#define CS_MCP3208 11
#define BUZ_PIN		  30

//- 함수 선언 ---------------------------------------------------------------------
void hw_init();
void parserCmd();
void InitBT();
void CaptureImage();
void sendSensorData();
void sendImageData();
void LedOn(int type, int bOn);
void BuzzerOn(int bOn);
int readAnalogData(int adcChannel);

//- 전역 변수 및 상수 -------------------------------------------------------------
enum { LED_RED, LED_GREEN, LED_BLUE};
static unsigned char LED[] = {27, 28, 29};
int potential, illum;
int bBuzzerRun =0;
char rx_buf[BUFSIZ] = "";
int  g_hBT;
int main(){
    unsigned int nNextTime, nBuzzTime, nSensorTime;
    InitBT();
    hw_init();
    nNextTime = millis();
    nBuzzTime = millis();
    nSensorTime = millis();
    while(1){
        if(millis() > nNextTime + 250){
		int i=0;
		memset(rx_buf, 0, sizeof(rx_buf));
		while(serialDataAvail(g_hBT) > 0){
				rx_buf[i] = serialGetchar(g_hBT);
				if(rx_buf[i] == '#'){
					if(rx_buf[0] == '@') parserCmd();
					break;
				}
				i++;
		}
              nNextTime = millis();
    }
    if(millis() > nSensorTime + 1000)
		{
			potential = readAnalogData(SPI_ADC_POTEN_CHANNEL);
			illum = readAnalogData(SPI_ADC_LIGHT_CHANNEL);
			printf("Send Sensor Data\n");
			sendSensorData();
			nSensorTime = millis();
		}
    }
    printf ("\n") ;
    return 0 ;
}
void hw_init(void){
    wiringPiSetup();
    if( wiringPiSPISetup (SPI_CE_CHANNEL, SPI_SPEED) == -1 ){
        fprintf (stderr, "wiringPiSPISetup Failed ! ERROR : %s\n", strerror (errno));
        return;
    }
	int i;
	for(i=0; i<sizeof(LED); i++){
		pinMode(LED[i], OUTPUT);
		digitalWrite(LED[i], LOW);
	}

	pinMode(BUZ_PIN,OUTPUT);
}
void parserCmd()
{
    int index = 1;
    if(!strncmp(&rx_buf[1], "BUZ", 3)) {
        index += 3;
        if(rx_buf[index] == ',')
        {
            int bOn;
            index++;
            if(rx_buf[index] == '0')
            {
		 printf("Buzzer off\n");
                bOn = 0;
            }else if(rx_buf[index] == '1'){
		 printf("Buzzer on\n");
                bOn = 1;
            }
            index++;
            if(rx_buf[index]== '#'){
				BuzzerOn(bOn);
                bBuzzerRun = bOn;
            }
        }
    }else if(!strncmp(&rx_buf[1], "RED", 3)) {
        index += 3;
        if(rx_buf[index] == ',')
        {
            int bOn = 0;
            index++;
            if(rx_buf[index] == '0')
            {
		 printf("Red LED off\n");
                bOn = 0;
            }else if(rx_buf[index] == '1'){
		 printf("Red LED on\n");
                bOn = 1;
            }

            index++;
            if(rx_buf[index]== '#')
                LedOn(LED_RED, bOn);
       }

    }else if(!strncmp(&rx_buf[1], "GED", 3)) {
        index += 3;
        if(rx_buf[index] == ',')
        {
            int bOn = 0;
            index++;
            if(rx_buf[index] == '0')
            {
		 printf("Green LED off\n");
                bOn = 0;
            }else if(rx_buf[index] == '1'){
		 printf("Green LED on\n");
                bOn = 1;
            }

            index++;
            if(rx_buf[index]== '#')
                LedOn(LED_GREEN, bOn);
        }
    }else if(!strncmp(&rx_buf[1], "BED", 3)){
        index += 3;
        if(rx_buf[index] == ',')
        {
            int bOn = 0;
            index++;
            if(rx_buf[index] == '0')
            {
		 printf("BLUE LED off\n");
                bOn = 0;
            }

			else if(rx_buf[index] == '1'){
		 printf("BLUE LED on\n");
                bOn = 1;
            }
            index++;
            if(rx_buf[index]== '#')
                LedOn(LED_BLUE, bOn);
        }
    }else if(!strncmp(&rx_buf[1], "WED", 3)){
        index += 3;
        if(rx_buf[index] == ',')
        {
            int bOn = 1;
            index++;
            if(rx_buf[index] == '0')
            {
		 printf("White LED off\n");
                bOn = 0;
            }else if(rx_buf[index] == '1'){
		 printf("White LED on\n");
                bOn = 1;
            }

            index++;
            if(rx_buf[index]== '#'){
                LedOn(LED_BLUE, bOn);
                LedOn(LED_GREEN, bOn);
                LedOn(LED_RED, bOn);
            }
        }
    }else{
	printf("%s\n", rx_buf);
        return;
    }
}

//- 블루투스 통신 초기화 -------------------------------------------------------
void InitBT()
{
	g_hBT = serialOpen("/dev/rfcomm0", 115200);
	printf("Bluetooth init\n");
}

//- 온도, 습도 전송 --------------------------------------------------------------
void sendSensorData()
{
	serialPrintf(g_hBT, "@POT,%d#@ILL,%d#", potential, illum);
}

//- LED On/Off 제어 -------------------------------------------------------------
void LedOn(int type, int bOn)
{
	digitalWrite(LED[type], bOn);
}
//- Buzzer On/Off 제어 ----------------------------------------------------------
void BuzzerOn(int bOn)
{
	if(bOn)
		digitalWrite(BUZ_PIN, HIGH);
	else
		digitalWrite(BUZ_PIN, LOW);
}
void CaptureImage(){
    return;
}
void sendImageData()
{
	char tx_buf[16];
	FILE *fp_read;
	long file_size;
	char *memblock;

	fp_read = fopen("", "rb");
	if(fp_read != NULL){
		fseek(fp_read, 0, SEEK_END);
		file_size = ftell(fp_read);
		memblock = (char*)malloc(file_size);
		fseek(fp_read, 0, SEEK_SET);
		fread(memblock, 1, file_size, fp_read);
		fclose(fp_read);

		serialPrintf(g_hBT, "@IMG,");
		write(g_hBT, memblock, file_size);
		serialPrintf(g_hBT, "####");
		free(memblock);
	}else{
		printf("File read error!\n");
		return;
	}
}
int readAnalogData(int adcChannel){
    int adcValue = 0;
    unsigned char buffer[3] = {0};
    //- MCP3008의 8개 채널 중 하나인지 체크
    if(adcChannel<0 || adcChannel>7) return -1;
    //- SPI 통신 버퍼 설정
    buffer[0] = 0x01;
    buffer[1] = (CHAN_CONFIG_SINGLE+adcChannel) << 4;
    //- SPI 통신할 칩 활성화
    digitalWrite(CS_MCP3208, LOW);
    //- 채널 1에서 측정된 ADC 센서 읽어 오기
    wiringPiSPIDataRW(SPI_CE_CHANNEL, buffer, 3);
    //- 읽어온 값에서 10비트 데이터 추출하기
    adcValue = ( (buffer[1] & 3 ) << 8 ) + buffer[2];
    //- SPI 통신할 칩 비활성화
    digitalWrite(CS_MCP3208, HIGH);
    return adcValue;
}