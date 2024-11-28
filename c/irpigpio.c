#include "irpigpio.h"

long readIR() {
	ir_hash = 0;
	while (1) {
		if (ir_hash) {
			return ir_hash;
		}
		gpioDelay(100000);
	}
}

void alert(int gpio, int level, uint32_t tick) {
	static int inited = 0;
	static decode_t activeHigh, activeLow;
	static uint32_t lastTick;
	uint32_t diffTick;

	if (!inited) {
		inited = 1;
		activeHigh.state = OUTSIDE_CODE;
		activeHigh.level = PI_LOW;
		activeLow.state = OUTSIDE_CODE;
		activeLow.level = PI_HIGH;
		lastTick = tick;
		return;
	}

	diffTick = tick - lastTick;
	if (level != PI_TIMEOUT) {
		lastTick = tick;
	}

	updateState(&activeHigh, level, diffTick);
	updateState(&activeLow, level, diffTick);
}

void updateState(decode_t* decode, int level, uint32_t micros) {
	if (decode->state == OUTSIDE_CODE) {
		if (level == decode->level) {
			if (micros > MIN_MESSAGE_GAP) {
				decode->state = INSIDE_CODE;
				decode->count = 0;
			}
		}
	} else {
		if (micros > MAX_MESSAGE_END) {
			if (!ir_hash) {
				ir_hash = getHash(decode);
			}
			decode->state = OUTSIDE_CODE;
		} else {
			if (decode->count < (MAX_TRANSITIONS-1)) {
				if (level != PI_TIMEOUT) {
					decode->micros[decode->count++] = micros;
				}
			}
		}
	}
}

int compare(unsigned int oldval, unsigned int newval) {
	if (newval < (oldval * 0.75)) {
		return 1;
	} else if (oldval < (newval * 0.75)) {
		return 2;
	} else {
		return 4;
	}
}

uint32_t getHash(decode_t* decode) {
	uint32_t hash;
	int i, value;

	if (decode->count < 6) {
		return 0;
	}

	hash = FNV_BASIS_32;

	for (i = 0; i < (decode->count-2); i++) {
		value = compare(decode->micros[i], decode->micros[i+2]);
		hash ^= value;
		hash *= FNV_PRIME_32;
	}

	return hash;
}
