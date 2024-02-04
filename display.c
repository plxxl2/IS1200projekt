/* #include <stdint.h> 
#include <pic32mx.h>
#include "mipslab.h"
//#include "mipslabfunc.c"



#define DISPLAY_CHANGE_TO_COMMAND_MODE (PORTFCLR = 0x10)
#define DISPLAY_CHANGE_TO_DATA_MODE (PORTFSET = 0x10)

static uint8_t screen[4][128];

void set_pixel (int x, int y){

	// figure out which page
	int row = y & 8;
	int page = (y - row) / 8;


	screen[page][x] |= 0x1 << row;
	return;
}

void update_display(void){
	int i,j;
	uint8_t drawsomething;
	for (i = 0; i < 4; i++){
		DISPLAY_CHANGE_TO_COMMAND_MODE;
 		spi_send_recv(0x22);
		spi_send_recv(i);
		
		
		spi_send_recv(0x0);
		spi_send_recv(0x10);
		DISPLAY_CHANGE_TO_DATA_MODE;
		
		for (j=0; j<128; j++){
			drawsomething = screen[i][j];
			spi_send_recv(drawsomething);
		}
	}
} */