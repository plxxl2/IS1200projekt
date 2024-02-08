/* mipslabfunc.c
   This file written 2015 by F Lundevall
   Some parts are original code written by Axel Isaksson

   For copyright and licensing, see file COPYING */

#include "display.h"

/* Declare a helper function which is local to this file */
//void num32asc( char * s, int, int ); 



/* quicksleep:
   A simple function to create a small delay.
   Very inefficient use of computing resources,
   but very handy in some special cases. */
void quicksleep(int cyc) {
	int i;
	for(i = cyc; i > 0; i--);
}

static uint8_t map[4][128];
static uint8_t screen[4][128];

void set_map_pixel (int x, int y){
	// figure out which page
	int row = y % 8;
	int page = (y - row) / 8;


	map[page][x] =map[page][x] |(0x1 << row);
	return;

}

void set_pixel (int x, int y){
	//check out of bounds
	if (x > 127 | x < 0 | y > 32 | y < 0) return;
	// figure out which page
	int row = y % 8;
	int page = (y - row) / 8;

	
	screen[page][x] =screen[page][x] |(0x1 << row);
	return;
}

void clear_display (void){
	int i,j;
	for (i = 0; i < 4; i++){
		for (j = 0; j < 128; j++){
			screen[i][j] = 0x00;
		}
	}
}

void set_map(void){
	int i,j;
	for (i=0;i<4;i++){
		for (j=0; j<128; j++){
			screen[i][j] = map[i][j];
		}
		
	}
}

void set_ball(int x, int y){
	int balldiameter = 3; //should be uneven atm
	int i, j;
	int edgedistance = (balldiameter - 1) / 2; // = 1
	for (i = 0; i < balldiameter; i++){ 
		for(j = 0; j < balldiameter; j++){ // 3x3 repeats
			//set_pixel(x + (i-1) * edgedistance, y + (j-1) * edgedistance;
			set_pixel(x + i - 1, y + j - 1);
			// set_pixel(x-1, y-1
		}
		
	}
}

void draw_aim(double ballx, double bally, double aim){
	int i;
	for (i=0; i < 7; i++){
		double distance = (double)(5 + i);
		int x = (int)(ballx + (distance*cos(aim)));
		int y = (int)(bally + (distance* sin(aim)));
		set_pixel(x, y);
	}
/*  	set_pixel ((int)ballx + 5, (int)bally);
	set_pixel ((int)ballx + 6, (int) bally);
	set_pixel ((int)ballx + 7, (int)bally);
	set_pixel ((int)ballx + 8, (int)bally);
	set_pixel ((int)ballx + 9, (int)bally);
	set_pixel ((int)ballx + 10, (int)bally);
	set_pixel ((int)ballx + 11, (int)bally);  */
}

/* void draw_pixel(int x, int y){
	int row = y % 8;
	int page = (y - row) / 8;
	
	DISPLAY_CHANGE_TO_COMMAND_MODE;
	// 96 = 1100000
	spi_send_recv(0x22);
	spi_send_recv(page);
	
	spi_send_recv(x); //tror detta är X koordinaten man börjar skriva på.
	// 1101111
	// 0
	spi_send_recv(0x10); // vet inte vad den här gör sätts till 8
	// 00010000 | (00000110 & 1111) == 00010000 | 00000110 = 00010110 = 22?
	// 111;
	//  0001000 | ((96 / 16) & 1111)
	//   110 & 1111 = 0110 = 6
	// 16 | 6 = 22
	//spi_send_recv(0x10 | ((x >> 4) & 0xF));
	// 0x10 | ((96 >> 4) & 0xF))
	// 0x10 | ((6) & 0xF))
	// 0x10 | (6 & 0xF)
	// 0x10 | (0b110 & 0xF)
	// 0x10 | (0b110 & 0b1111)
	// 0x10 | (0b110)
	// 0b00010000 | 0b110
	// 0b10000 | 0b110
	// 0b10110
	// 2+4+16 = 22
	DISPLAY_CHANGE_TO_DATA_MODE;
	
	spi_send_recv(~0b1<<row);
	//for(j = 0; j < 32; j++)
		//spi_send_recv(~data[i*32 + j]);

} */



/* void display_myimage(int x) {
	int i, j;
	
	for(i = 0; i < 4; i++) {
		DISPLAY_CHANGE_TO_COMMAND_MODE;
		// 96 = 1100000
		spi_send_recv(0x22);
		spi_send_recv(i);
		
		spi_send_recv(0);
		// 1101111
		// 96 + 7
		spi_send_recv(0x10 | ((x >> 4) & 0xF));
		// 0001000 | (0000110 & 1111)
		// 111;
		//  00010000 | ((96 / 16) & 1111)
		//   110 & 1111 = 0110 = 6
		// 00010000 | 0110
		// 00010110
		// 8 | 6 = 14
		DISPLAY_CHANGE_TO_DATA_MODE;
		
		for(j = 0; j < 32; j++)
			spi_send_recv(~255);  //0b 1111
	}
} */


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
			// we wanna invert here
			
			drawsomething = reverse(screen[3-i][j]);
			//drawsomething = screen[3-i][j];
			//drawsomething = reverse(drawsomething);
			spi_send_recv(drawsomething);
		}
	}
}

/* uint8_t reverse (uint8_t input){
	uint8_t output = 0;
	int i;
	for (i=0; i < 8; i++){
		int a = (input >> i ) & 0x01;
		if (a == 1) output = output | a << (7-i);
	}
	return output;
} */
/* tick:
   Add 1 to time in memory, at location pointed to by parameter.
   Time is stored as 4 pairs of 2 NBCD-digits.
   1st pair (most significant byte) counts days.
   2nd pair counts hours.
   3rd pair counts minutes.
   4th pair (least significant byte) counts seconds.
   In most labs, only the 3rd and 4th pairs are used. */
// void tick( unsigned int * timep )
// {
  // /* Get current value, store locally */
  // register unsigned int t = * timep;
  // t += 1; /* Increment local copy */
  
  // /* If result was not a valid BCD-coded time, adjust now */

  // if( (t & 0x0000000f) >= 0x0000000a ) t += 0x00000006;
  // if( (t & 0x000000f0) >= 0x00000060 ) t += 0x000000a0;
  // /* Seconds are now OK */

  // if( (t & 0x00000f00) >= 0x00000a00 ) t += 0x00000600;
  // if( (t & 0x0000f000) >= 0x00006000 ) t += 0x0000a000;
  // /* Minutes are now OK */

  // if( (t & 0x000f0000) >= 0x000a0000 ) t += 0x00060000;
  // if( (t & 0x00ff0000) >= 0x00240000 ) t += 0x00dc0000;
  // /* Hours are now OK */

  // if( (t & 0x0f000000) >= 0x0a000000 ) t += 0x06000000;
  // if( (t & 0xf0000000) >= 0xa0000000 ) t = 0;
  // /* Days are now OK */

  // * timep = t; /* Store new value */
// }

/* display_debug
   A function to help debugging.

   After calling display_debug,
   the two middle lines of the display show
   an address and its current contents.

   There's one parameter: the address to read and display.

   Note: When you use this function, you should comment out any
   repeated calls to display_image; display_image overwrites
   about half of the digits shown by display_debug.
*/   
// void display_debug( volatile int * const addr )
// {
  // display_string( 1, "Addr" );
  // display_string( 2, "Data" );
  // num32asc( &textbuffer[1][6], (int) addr );
  // num32asc( &textbuffer[2][6], *addr );
  // display_update();
// }

uint8_t spi_send_recv(uint8_t data) {
	while(!(SPI2STAT & 0x08));
	SPI2BUF = data;
	while(!(SPI2STAT & 1));
	return SPI2BUF;
}

void display_init(void) {
        DISPLAY_CHANGE_TO_COMMAND_MODE;
	quicksleep(10);
	DISPLAY_ACTIVATE_VDD;
	quicksleep(1000000);
	
	spi_send_recv(0xAE);
	DISPLAY_ACTIVATE_RESET;
	quicksleep(10);
	DISPLAY_DO_NOT_RESET;
	quicksleep(10);
	
	spi_send_recv(0x8D);
	spi_send_recv(0x14);
	
	spi_send_recv(0xD9);
	spi_send_recv(0xF1);
	
	DISPLAY_ACTIVATE_VBAT;
	quicksleep(10000000);
	
	spi_send_recv(0xA1);
	spi_send_recv(0xC8);
	
	spi_send_recv(0xDA);
	spi_send_recv(0x20);
	
	spi_send_recv(0xAF);
}

void display_string(int line, char *s) {
	int i;
	if(line < 0 || line >= 4)
		return;
	if(!s)
		return;
	
	for(i = 0; i < 16; i++)
		if(*s) {
			textbuffer[line][i] = *s;
			s++;
		} else
			textbuffer[line][i] = ' ';
}

/* void set_Char (int line, int position, char *s){ //doesnt work anyways
	textbuffer[line][position] = s;
		if(s) textbuffer[line][position] = s;
	else textbuffer[line][position] = ' '; 
} */

void draw_walls(struct wall walls[]){
	int length = sizeof(walls) / sizeof(walls[0]);
	int i, j;
	for (i = 0; i < length; i++){
		for (j = 0; j < walls[i].length; j++){
			set_pixel( walls[i].x + cos(walls[i].direction)   , walls[i].y + sin(walls[i].direction));
		}
	}
}


void display_image(int x, const uint8_t *data) {
	int i, j;
	
	for(i = 0; i < 4; i++) {
		DISPLAY_CHANGE_TO_COMMAND_MODE;
		// 96 = 1100000
		spi_send_recv(0x22);
		spi_send_recv(i);
		
		spi_send_recv(x & 0xF);
		// 1101111
		// 96 + 7
		spi_send_recv(0x10 | ((x >> 4) & 0xF));
		// 0001000 | (0000110 & 1111)
		// 111;
		//  0001000 | ((96 / 16) & 1111)
		//   110 & 1111 = 0110 = 6
		// 8 | 6 = 14
		DISPLAY_CHANGE_TO_DATA_MODE;
		
		for(j = 0; j < 32; j++)
			spi_send_recv(~data[i*32 + j]);
	}
}

/* void display_ball (int x, int y){
	int i, j;
	
	for(i = 0; i < 4; i++) {
	//	DISPLAY_CHANGE_TO_COMMAND_MODE;
		// 96 = 1100000
	//	spi_send_recv(0x22);
	//	spi_send_recv(i);
		
	//	spi_send_recv(x & 0xF);
		// 1101111
	//	spi_send_recv(0x10 | ((x >> 4) & 0xF));
		// 0001000 | 0000110
		DISPLAY_CHANGE_TO_DATA_MODE;
		
		for(j = 0; j < 128; j++)
			spi_send_recv(~255);
	}
	
	
} */


void display_update(void) {
	int i, j, k;
	int c;
	for(i = 0; i < 4; i++) {
		DISPLAY_CHANGE_TO_COMMAND_MODE;
		spi_send_recv(0x22);
		spi_send_recv(i);
		
		spi_send_recv(0x0);
		spi_send_recv(0x10);
		
		DISPLAY_CHANGE_TO_DATA_MODE;
		
		for(j = 0; j < 16; j++) {
			c = textbuffer[i][j];
			if(c & 0x80)
				continue;
			
			for(k = 0; k < 8; k++)
				spi_send_recv(font[c*8 + k]);
		}
	}
}

/* Helper function, local to this file.
   Converts a number to hexadecimal ASCII digits. */
// void num32asc( char * s, int n ) 
// {
  // int i;
  // for( i = 28; i >= 0; i -= 4 )
    // *s++ = n;//"0123456789ABCDEF"[ (n >> i) & 15 ];
// }


// void num32asc( char * s, int n, int sz ) 
// {
  // int i;
  // int bits;
  // if( sz == 4 ) bits = 28;
  // else if( sz == 2 ) bits = 12;
  // else if( sz == 1 ) bits = 4;
  // else bits = 0;
  // if( bits )
    // for( i = bits; i >= 0; i -= 4 )
      // *s++ = "0123456789ABCDEF"[ (n >> i) & 15 ];
  // else
    // for( i = 0; i < 8; i += 1 )
      // *s++ = "bad size"[ i ];
// }


/*
 * nextprime
 * 
 * Return the first prime number larger than the integer
 * given as a parameter. The integer must be positive.
 */
#define PRIME_FALSE   0     /* Constant to help readability. */
#define PRIME_TRUE    1     /* Constant to help readability. */
int nextprime( int inval )
{
   register int perhapsprime = 0; /* Holds a tentative prime while we check it. */
   register int testfactor; /* Holds various factors for which we test perhapsprime. */
   register int found;      /* Flag, false until we find a prime. */

   if (inval < 3 )          /* Initial sanity check of parameter. */
   {
     if(inval <= 0) return(1);  /* Return 1 for zero or negative input. */
     if(inval == 1) return(2);  /* Easy special case. */
     if(inval == 2) return(3);  /* Easy special case. */
   }
   else
   {
     /* Testing an even number for primeness is pointless, since
      * all even numbers are divisible by 2. Therefore, we make sure
      * that perhapsprime is larger than the parameter, and odd. */
     perhapsprime = ( inval + 1 ) | 1 ;
   }
   /* While prime not found, loop. */
   for( found = PRIME_FALSE; found != PRIME_TRUE; perhapsprime += 2 )
   {
     /* Check factors from 3 up to perhapsprime/2. */
     for( testfactor = 3; testfactor <= (perhapsprime >> 1) + 1; testfactor += 1 )
     {
       found = PRIME_TRUE;      /* Assume we will find a prime. */
       if( (perhapsprime % testfactor) == 0 ) /* If testfactor divides perhapsprime... */
       {
         found = PRIME_FALSE;   /* ...then, perhapsprime was non-prime. */
         goto check_next_prime; /* Break the inner loop, go test a new perhapsprime. */
       }
     }
     check_next_prime:;         /* This label is used to break the inner loop. */
     if( found == PRIME_TRUE )  /* If the loop ended normally, we found a prime. */
     {
       return( perhapsprime );  /* Return the prime we found. */
     } 
   }
   return( perhapsprime );      /* When the loop ends, perhapsprime is a real prime. */
} 

/*
 * itoa
 * 
 * Simple conversion routine
 * Converts binary to decimal numbers
 * Returns pointer to (static) char array
 * 
 * The integer argument is converted to a string
 * of digits representing the integer in decimal format.
 * The integer is considered signed, and a minus-sign
 * precedes the string of digits if the number is
 * negative.
 * 
 * This routine will return a varying number of digits, from
 * one digit (for integers in the range 0 through 9) and up to
 * 10 digits and a leading minus-sign (for the largest negative
 * 32-bit integers).
 * 
 * If the integer has the special value
 * 100000...0 (that's 31 zeros), the number cannot be
 * negated. We check for this, and treat this as a special case.
 * If the integer has any other value, the sign is saved separately.
 * 
 * If the integer is negative, it is then converted to
 * its positive counterpart. We then use the positive
 * absolute value for conversion.
 * 
 * Conversion produces the least-significant digits first,
 * which is the reverse of the order in which we wish to
 * print the digits. We therefore store all digits in a buffer,
 * in ASCII form.
 * 
 * To avoid a separate step for reversing the contents of the buffer,
 * the buffer is initialized with an end-of-string marker at the
 * very end of the buffer. The digits produced by conversion are then
 * stored right-to-left in the buffer: starting with the position
 * immediately before the end-of-string marker and proceeding towards
 * the beginning of the buffer.
 * 
 * For this to work, the buffer size must of course be big enough
 * to hold the decimal representation of the largest possible integer,
 * and the minus sign, and the trailing end-of-string marker.
 * The value 24 for ITOA_BUFSIZ was selected to allow conversion of
 * 64-bit quantities; however, the size of an int on your current compiler
 * may not allow this straight away.
 */
#define ITOA_BUFSIZ ( 24 )
char * itoaconv( int num )
{
  register int i, sign;
  static char itoa_buffer[ ITOA_BUFSIZ ];
  static const char maxneg[] = "-2147483648";
  
  itoa_buffer[ ITOA_BUFSIZ - 1 ] = 0;   /* Insert the end-of-string marker. */
  sign = num;                           /* Save sign. */
  if( num < 0 && num - 1 > 0 )          /* Check for most negative integer */
  {
    for( i = 0; i < sizeof( maxneg ); i += 1 )
    itoa_buffer[ i + 1 ] = maxneg[ i ];
    i = 0;
  }
  else
  {
    if( num < 0 ) num = -num;           /* Make number positive. */
    i = ITOA_BUFSIZ - 2;                /* Location for first ASCII digit. */
    do {
      itoa_buffer[ i ] = num % 10 + '0';/* Insert next digit. */
      num = num / 10;                   /* Remove digit from number. */
      i -= 1;                           /* Move index to next empty position. */
    } while( num > 0 );
    if( sign < 0 )
    {
      itoa_buffer[ i ] = '-';
      i -= 1;
    }
  }
  /* Since the loop always sets the index i to the next empty position,
   * we must add 1 in order to return a pointer to the first occupied position. */
  return( &itoa_buffer[ i + 1 ] );
}
