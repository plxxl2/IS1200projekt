#include <stdint.h>
#include <pic32mx.h>
#include "mipslab.h" 

int getsw (void)
{
	return (PORTD / 256) & 0b1111;
}

int getbtns(void)
{
	return  (PORTD / 32) & 0b111;
}



