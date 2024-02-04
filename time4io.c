#include <stdint.h>
#include <pic32mx.h>
#include "mipslab.h" 

int getsw (void)
{
	return (PORTD / 256) & 0b1111;
}

int getbtns(void)
{
	//return  (PORTD / 16) & 0b1111;
	//return  (PORTD / 32) & 0b111;
	//((PORTF >> 1) & 1)
	return  (PORTD >> 4) & 0b1110 | ((PORTF >> 1) & 1);
}


 //0xFFFF
 
 ///32 = 2*2*2*2*2 == >>5
 
 
 //111xxxxx
