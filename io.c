


#include "io.h"

void set_led(int input){ //allowed inputs 1-8, turns on that led
	volatile int* leds = (volatile int*) 0xbf886110;
	//PORTFSET = 7 - (input - 1);
	
	
	*leds = *leds | (1 << (8 - input));
}

	




void reset_led(int input){ //allowed inputs 1-8, turns off that led
	//PORTFCLR = 7 - (input - 1);
	volatile int* leds = (volatile int*) 0xbf886110;
	*leds = *leds & (0xFFFFFFFF - (1 << (8 - input)));
}


void reset_led_all(void){ //turns of all leds
	volatile int* leds = (volatile int*) 0xbf886110;
	*leds = *leds & 0xFFFFFF00;
}

void set_led_all(void){ //turns on all leds
	volatile int* leds = (volatile int*) 0xbf886110;
	*leds = *leds | 0x000000FF;
	//PORTFSET = 0b11111111;
}