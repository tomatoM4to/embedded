#include <stdio.h>
#include <wiringPi.h>
#include <softTone.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <wiringSerial.h>
#include <pthread.h>

// Pin definitions
#define BUZ_P_PIN 21  // PWM Buzzer pin
#define BUZ_PIN 30    // Simple Buzzer pin

// Musical notes frequencies (Hz)
#define NOTE_C4 262  // 도
#define NOTE_D4 294  // 레
#define NOTE_E4 330  // 미
#define NOTE_F4 349  // 파
#define NOTE_G4 392  // 솔
#define NOTE_A4 440  // 라
#define NOTE_B4 494  // 시
#define NOTE_C5 523  // 높은 도

// Buffer size
#define BUFSIZ 1024

// Music patterns
#define MAX_NOTES 100

// Songs
int rabbitSong[] = {
    NOTE_C4, NOTE_C4, NOTE_E4, NOTE_E4, NOTE_G4, NOTE_G4, NOTE_E4,
    NOTE_E4, NOTE_E4, NOTE_C4, NOTE_E4, NOTE_G4, NOTE_E4,
    NOTE_C4, NOTE_C4, NOTE_E4, NOTE_E4, NOTE_G4, NOTE_G4, NOTE_E4,
    NOTE_E4, NOTE_C4, NOTE_E4, NOTE_C4, -1
};

int starSong[] = {
    NOTE_C4, NOTE_C4, NOTE_G4, NOTE_G4, NOTE_A4, NOTE_A4, NOTE_G4,
    NOTE_F4, NOTE_F4, NOTE_E4, NOTE_E4, NOTE_D4, NOTE_D4, NOTE_C4,
    NOTE_G4, NOTE_G4, NOTE_F4, NOTE_F4, NOTE_E4, NOTE_E4, NOTE_D4,
    NOTE_G4, NOTE_G4, NOTE_F4, NOTE_F4, NOTE_E4, NOTE_E4, NOTE_D4,
    NOTE_C4, NOTE_C4, NOTE_G4, NOTE_G4, NOTE_A4, NOTE_A4, NOTE_G4,
    NOTE_F4, NOTE_F4, NOTE_E4, NOTE_E4, NOTE_D4, NOTE_D4, NOTE_C4, -1
};

// Function declarations
void hw_init(void);
void buzzerInit(void);
void parseCommand(void);
void initBluetooth(void);
void buzzerStop(void);
void playNote(char note);
void *playSongThread(void *arg);
void startSong(int *notes);

// Global variables
char rx_buf[BUFSIZ];
int bluetoothHandle;
int isPlaying = 0;
pthread_t musicThread;
int currentSong[MAX_NOTES];
int stopFlag = 0;

int main() {
    unsigned int nextTime;

    if (wiringPiSetup() == -1) {
        fprintf(stderr, "Unable to initialize WiringPi: %s\n", strerror(errno));
        return 1;
    }

    hw_init();
    initBluetooth();

    printf("Bluetooth Buzzer Server Started\n");
    printf("Waiting for commands...\n");

    nextTime = millis();

    while(1) {
        if(millis() > nextTime + 100) {
            int i = 0;
            memset(rx_buf, 0, sizeof(rx_buf));

            while(serialDataAvail(bluetoothHandle) > 0) {
                rx_buf[i] = serialGetchar(bluetoothHandle);
                if(rx_buf[i] == '#') {
                    if(rx_buf[0] == '@') {
                        parseCommand();
                    }
                    break;
                }
                i++;
            }
            nextTime = millis();
        }
        delay(1);
    }

    return 0;
}

void hw_init(void) {
    buzzerInit();
    digitalWrite(BUZ_PIN, LOW);
    softToneWrite(BUZ_P_PIN, 0);
}

void buzzerInit(void) {
    pinMode(BUZ_PIN, OUTPUT);
    pinMode(BUZ_P_PIN, SOFT_TONE_OUTPUT);
    softToneCreate(BUZ_P_PIN);
    printf("Buzzer initialized\n");
}

void initBluetooth(void) {
    bluetoothHandle = serialOpen("/dev/rfcomm0", 115200);
    if(bluetoothHandle < 0) {
        fprintf(stderr, "Unable to open Bluetooth device: %s\n", strerror(errno));
        exit(1);
    }
    printf("Bluetooth initialized\n");
}

void parseCommand(void) {
    if(strncmp(&rx_buf[1], "BUZ,", 4) == 0) {
        char note = rx_buf[5];

        // Special commands for songs
        if(note == '1') {  // 산토끼
            startSong(rabbitSong);
        }
        else if(note == '2') {  // 작은별
            startSong(starSong);
        }
        else if(note == 'k') {  // Stop
            stopFlag = 1;
            buzzerStop();
        }
        else {  // Individual notes
            playNote(note);
        }
    }
}

void *playSongThread(void *arg) {
    int *notes = (int *)arg;
    int i = 0;
    stopFlag = 0;

    while(notes[i] != -1 && !stopFlag) {
        softToneWrite(BUZ_P_PIN, notes[i]);
        delay(500);  // Note duration
        i++;
    }

    buzzerStop();
    pthread_exit(NULL);
}

void startSong(int *notes) {
    // Stop any currently playing song
    stopFlag = 1;
    delay(100);  // Give time for the previous thread to stop

    // Copy the song to current song buffer
    int i;
    for(i = 0; notes[i] != -1 && i < MAX_NOTES; i++) {
        currentSong[i] = notes[i];
    }
    currentSong[i] = -1;

    // Create new thread to play the song
    if(pthread_create(&musicThread, NULL, playSongThread, (void *)currentSong) != 0) {
        fprintf(stderr, "Error creating thread\n");
        return;
    }

    // Detach the thread so it can clean up itself
    pthread_detach(musicThread);
}

void playNote(char note) {
    stopFlag = 1;  // Stop any playing song
    delay(50);     // Small delay to ensure song stops

    switch(note) {
        case 'a':  // 도
            softToneWrite(BUZ_P_PIN, NOTE_C4);
            break;
        case 's':  // 레
            softToneWrite(BUZ_P_PIN, NOTE_D4);
            break;
        case 'd':  // 미
            softToneWrite(BUZ_P_PIN, NOTE_E4);
            break;
        case 'f':  // 파
            softToneWrite(BUZ_P_PIN, NOTE_F4);
            break;
        case 'g':  // 솔
            softToneWrite(BUZ_P_PIN, NOTE_G4);
            break;
        case 'h':  // 라
            softToneWrite(BUZ_P_PIN, NOTE_A4);
            break;
        case 'j':  // 시
            softToneWrite(BUZ_P_PIN, NOTE_B4);
            break;
        case 'q':  // 단순 부저
            digitalWrite(BUZ_PIN, HIGH);
            break;
        default:
            buzzerStop();
            break;
    }
}

void buzzerStop(void) {
    softToneWrite(BUZ_P_PIN, 0);
    digitalWrite(BUZ_PIN, LOW);
    isPlaying = 0;
}