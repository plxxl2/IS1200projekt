
#include "misc.h"



uint8_t reverse (uint8_t input){ //The bitmap in the display starts in upper left corner, contrary to "common" coordinates, this funcntion can reverse the array representing the bits of the display to allow 0,0 to be bottom left.
	uint8_t output = 0;
	int i;
	for (i=0; i < 8; i++){
		int a = (input >> i ) & 0x01;
		if (a == 1) output = output | a << (7-i);
	}
	return output;
}

int absolute(int a){ //gives absolute values for calculating vector distances between coordinates.
	if (a < 0) return -a;
	else return a;
}

double absoluted(double a){ //gives absolute values for calculating vector distances between coordinates, but its a double
	if (a < 0) return -a;
	else return a;
}