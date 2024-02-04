/* mipslabwork.c
asdf
   This file written 2015 by F Lundevall
   Updated 2017-04-21 by F Lundevall

   This file should be changed by YOU! So you must
   add comment(s) here with your name(s) and date(s):

   This file modified 2017-04-31 by Ture Teknolog 

   For copyright and licensing, see file COPYING */

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "mipslab.h"  /* Declatations for these labs */
#include <stdio.h>
#include <math.h>
#include "golf.h"


#define PI 3.14159
#define MIN_BALL_SPEED 0.2

int startcount = 50; //some of these, (at least ballsize) can be defines instead, some are leftovers from labs. (textstring at least)
double ballx = 16;
double bally = 25;
double balldx, balldy;
double balldirection;
double aim = PI/2;
double ballvelocity = 1;
int ballsize = 3;
int totalscore;
int currentscore;
uint8_t collisionmap[32][128];
int x = 1;
uint8_t charge = 0;
uint8_t chargingup;
int timeoutcount = 50;
int introtimer = 10;
char textstring[] = "text, more text, and even more text!";
static enum gamestate current_game_state = aiming;
static enum gamestate previous_game_state;
static enum menustate current_menu_state = intro;

void moveball( void ){
	//ballx+= ballvelocity * cos(balldirection);
	//bally+= ballvelocity * sin(balldirection);
	ballx+= ballvelocity * balldx;
	bally+= ballvelocity * balldy;
	ballvelocity = ballvelocity * BALL_SLOWDOWN;
}	

/* Interrupt Service Routine */
void user_isr( void )
{
	IFSCLR(0) = 0x100;
	/* timeoutcount--;
	if (timeoutcount != 0){
		return;
	}
	timeoutcount = 1; */
	//moveball();
	//clear_display();
	//display_image(ballx, ballimage);
	//display_ball(ballx, bally);
	//display_update();
	//if (x != 0) x--;
	
	//if (ballx > 127) ballx = 16;
	switch(current_menu_state){
		case(intro):
				timeoutcount--;
				if (timeoutcount != 0){
					break;
				}
				introtimer--;
				timeoutcount = 50;
				display_string(3, itoaconv(introtimer));
				display_update();
				if(introtimer != 0){
					break;
				}
				current_menu_state = playing;
				
			break;
		case(menu):
				
			break;
		case(playing):
				advance_game();
			break;
		case(scorecard):
			set_scorecard();
			display_update();
			break;
		//default:	
	
	}
	
	
/* 	for (i = 0; i < 32; i++){
		for (j = 0; j < 1; j++){
			//if (((j+x) % 2) == 0){
				//set_pixel (j,i);
			//}
		}
	} */
	//update_display();
}

void set_scorecard( void ){ //Updates the scorecard text
	display_string(0, "Score: ");
	//num32asc( &textbuffer[0][8], itoaconv(currentscore) );
	//set_Char (0, 4, itoaconv(currentscore));
	display_string(1, itoaconv(currentscore));
	display_string(2, "Total: ");
	display_string(3, itoaconv(totalscore + currentscore));

}

void advance_game( void){ //advances the game 1 frame. om gamestate = aiming, ritar sikte, flyttar sikte om knapper trycks. om gamestate = charging, ändrar charge variablen + medföljande lampor
// om gamestate = moving, flyttar bollen, samt går tillbaka till aiming om bollens hastighet är låg.
// alla gamestates: ritar nästa frame på skärmen.
	int i,j;
	//display_myimage(96);
	//draw_pixel(5,5);
	//draw_pixel(5,10);
	//clear_display();
	volatile int* leds = (volatile int*) 0xbf886110;
	int btns= getbtns(); 
	set_map(); //nollställer displayen till mappen
	switch (current_game_state){
		case(aiming):
				if ((btns & 4 ) == 4){
					aim+= PI/180;
				}
				if ((btns & 2 )== 2){
					aim-= PI/180;
				}
				draw_aim(ballx, bally, aim);
				//draw_aim(ballx, bally, (M_PI / 2));
				//draw_aim(ballx, bally, (M_PI / 4)); //tests to see lines
			break;
		case(charging):
			timeoutcount--;
			if (timeoutcount != 0){
				break;
			}
			timeoutcount = 5;
			if ((btns & 8 ) == 8){ //charge button is being held down, we increase the charge
				if (charge == 8) chargingup = 0;
				if (charge == 0) chargingup = 1;
				if (chargingup == 1){
					charge+= 1;
					*leds = *leds | 1 <<(8-charge);
				} else{
					charge-= 1;
					*leds = *leds & (0xFF & (0xFF << (8-charge))); // charge = 6 => & 11111110  charge = 5 => && 11111100 charge = 0 7-charge 0s
				}
				
			} else{
				ballvelocity = 0.1 + (0.2 * charge); //this should be a variable depending on charge to hit harder/softer
				charge = 0;
				balldirection = aim;
				balldx = cos(aim);
				balldy = sin(aim);
				*leds = *leds & 0x00;
				//next_state = moving;
				current_game_state = moving;
				currentscore++;
			}
			break;
		case(moving):
			moveball();
			if (ballvelocity < MIN_BALL_SPEED){
				ballvelocity = 0;
				current_game_state = aiming;
			}
			break;
			
		//default:
			//default stuff if no state
	}
	 
	set_ball((int)(ballx + 0.5), (int)(bally+0.5)); //runder upp bollkoordinater om dom är över  x.5.
	update_display();
}

void load_map(void){
	int i,j,k;
	int mapsizeX = 128/ 8; // = 16
	int mapsizeY;
	for (i = 0; i < 32; i++){
		for (j = 0; j < 16; j++){
			int z = map1[i*mapsizeX + j];
			//collisionmap[i][j] = z; //loads collisions
			if (z > 0) {
				for (k=0; k<8; k++){
					if (((z >> (7-k)) & 1) == 1){
						collisionmap[i][j*8+k] = z; //loads collisions
						set_map_pixel((j*8+k), i);
					}
				}
			
			}
			
		}
		
	}
	//add collision tangents to collisionmap 
}

/* Lab-specific initialization goes here */
void labinit( void )
{
	volatile int* leds = (volatile int*) 0xbf886100;
	*leds = *leds & 0x00;
	//volatile int* inputs = (volatile int*) addresss;
	//*inputs = *inputs & 4064; // 0b111111100000
	TRISD = TRISD | 4064;
	TRISFSET = 1;
	// TRISE = TRISE (0xbf886100) | 0xFF; // Means TRISE xxxxxxxxxxxxxxxxxxxxxxxxx -> xxxxxxxxxxxxxxxxx11111111
	//T2CON = 15 = 1  6-4 = 111 0x8070   15 14 13 12  11 10 9 8    7 6 5 4   3 2 1 0
	TMR2 = 0;  // reset timer register
	T2CON = 0x0; // reset timer config
	T2CONSET = 0b111 << 4; // Set scaling to 1:256
	//PR2 = 0x7FFF; // set period to max (or not) = 0b0111111111111111 = about 32000 * 256 from scaling = about 8 million. 80Mhz sample rate => 10 timers per second, timer = 100 ms  Should be reduced by a few % to make it exact
	//11000011010100
	//3	0	B	4
	//PR2 = 0x30B4; // = 12500 *256 = 3200000.    80M / 3,2M = 25fps
	
	//1100001101010
	//18   6   A
	//PR2 = 0x186A; // for 50 fps
	//110000110101
	//C   4   5
	PR2=0x0C45; // 100 fps
	IPCSET(2) = 0x0000000C; //set priority level   har ingen aning om vad detta gör :)
	IFSCLR(0) = 0x100;
	IECSET(0) = 0x100;
	//enable
	T2CONSET  = 0x8000;
	
	
	
	
	
	
	
	//load map 1
	load_map();
	
	
	
	
	
	enable_interrupt();
		
	
  return;
}






int check_outofboundsY(void){
	if (bally > 30 & sin(balldirection) > 0 | bally < 1 & sin(balldirection) < 0) return 1;
}

void ball_bounce(double walldirection){
	double a = balldirection;
	double b = walldirection + (PI/2); // normal till väggen
	
	//double ax = cos(a);
	//double ay = sin(a); // vi skapar 2 vektorer, bollens riktnning a, samt väggens normal b
	double ax = balldx;
	double ay = balldy;
	double bx = cos(b);
	double by = sin(b);

	double nv = (bx * ax) + (by * ay); //punktprodukt av a ' b
	
	//double x = -(2*nv*bx) + ax;  // skapar vektorn (a,y) som är reflektionen till a genom b
	//double y = -(2*nv*by) + ay;
	
	double x = ax - (2*nv*bx);
	double y = ay - (2*nv*by);
	balldx = x;
	balldy = y;
	//double x = -2*((cos(b) * cos(a) + sin(b) * sin(a)) * cos(b) - cos(a));
	//double y = -2*((cos(b) * cos(a) + sin(b) * sin(a)) * sin(b) - sin(a));
	//double x =  - (2 *  ((  (cos(b) * cos(a)) + (sin(b) * sin(a))  )   * cos(b)) - cos(a));
	//double y =  - (2 *  ((  (cos(b) * cos(a)) + (sin(b) * sin(a))  )   * sin(b)) - sin(a));
	//-  (2 *  (  cos(b) * cos(a) + sin(b) * sin(a)  )   * sin(b)) - sin(a);
	//balldirection = atan2(y, x); //arctan ger oss 
	//balldirection = balldirection + M_PI; //temp, real =  
	//balldirection += 2*( walldirection-balldirection);
}

void check_outofboundsCol(void){
	if (ballx > 125 & balldx > (double)0 | ballx < 1 & balldx < (double)0 )  ball_bounce(PI/2);    //balldx = -balldx;  simplet alternativ
	if (bally > 30 & balldy > (double)0 | bally < 1 & balldy < (double)0)   ball_bounce(PI);                 //balldy = -balldy;
}

int edgecollision (void){
	int i;
	//for (i = 0; i < collisionmap.len; i++){
	//	if ball.
	//}

}

void check_collision(void){
	check_outofboundsCol();
	//if (bounce == 1){
		//ball_bounce();
	//	return;
	//}

}

/* This function is called repetitively from the main program */
void labwork( void )
{
	//int potentio = analogRead(A0);
	volatile int* leds = (volatile int*) 0xbf886110;
	//static enum gamestate current_game_state = aiming;
	enum gamestate next_state;
	int btns= getbtns(); 
	if (current_menu_state == intro) return;
	if ((btns & 1) == 1) {
		*leds = *leds | 1;
		current_menu_state = scorecard;
		//current_game_state = scorecard;
	} else {
		*leds = *leds & 0xFE;
		current_menu_state = playing;
		//current_game_state = previous_game_state; // value is not restored
	}
	
	switch(current_game_state){
			case(aiming):
				//if (ballx > 128 | ballx < 0) ballx = 16;
				//if (bally > 32 | bally < 0) bally = 16;
				// Positon i koden; då addition på mytime kan gå out of bounds av klockans bas 60, och det hanteras av tick(), så vill vi att tick() kallas mellan denna addition & displayupdate
				if ((btns & 8 ) == 8){ // 4 -> 8
					//*leds = *leds | (btns*16); // old test function, led 3 + 4 = 7 lights up
					charge = 0;
					timeoutcount = 5;
					chargingup = 1;
					next_state = charging;
					current_game_state = next_state;
					

				}else{

				}
				
				break;
			
			case(charging):
				if ((btns & 8 ) == 8){ //charge button is being held down, we increase the charge
					//if (charge == 7) chargingup = 0;
					//if (charge == 0) chargingup = 1;
					//if (chargingup == 1){
					//	charge+= 1;
					//	*leds = *leds | 1 <<(7-7);
					//} else{
					//	charge-= 1;
					//	*leds = *leds & 1 <<(7-7);
					//}
					//*leds = *leds | ((2^(8-charge))>>1);
				}else{ //Charge button is not being held down, we shoot the ball
					/* ballvelocity = 0.5 + (charge/7); //this should be a variable depending on charge to hit harder/softer
					charge = 0;
					balldirection = aim;
					balldx = cos(aim);
					balldy = sin(aim);
					*leds = *leds & 0x00;
					next_state = moving;
					current_game_state = next_state; */
				}
					//*leds = *leds & (btns*16);
				break;
			case(moving):
				check_collision();
			
			
				break;
	}	
	
	
	//prime = nextprime(prime);
	//display_string(0, itoaconv(prime));
	//display_update();
	
	
	//*asdf = *asdf + 0x01; //Tidig del övning att binärt räkna ticks på LEDs, avstängd pga felsöknings funktion nedan
}

//enum gamestate updatestate(