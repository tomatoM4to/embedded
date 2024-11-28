#ifndef irpigpio_h
#define irpigpio_h

#include <pigpio.h>

#define OUTSIDE_CODE		0
#define INSIDE_CODE		1

#define MIN_MESSAGE_GAP		3000
#define MAX_MESSAGE_END		3000

#define MAX_TRANSITIONS		500

#define FNV_PRIME_32		16777619
#define FNV_BASIS_32		2166136261U

static volatile uint32_t ir_hash = 0;

typedef struct {
	int state, count, level;
	uint16_t micros[MAX_TRANSITIONS];
} decode_t;

long readIR();
void alert(int, int, uint32_t);
void updateState(decode_t*, int, uint32_t);
int compare(unsigned int, unsigned int);
uint32_t getHash(decode_t*);

#endif
