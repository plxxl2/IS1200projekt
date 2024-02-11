/* mipslabwork.c
asdf
   This file written 2015 by F Lundevall
   Updated 2017-04-21 by F Lundevall

   This file should be changed by YOU! So you must
   add comment(s) here with your name(s) and date(s):

   This file modified 2017-04-31 by Ture Teknolog 

   For copyright and licensing, see file COPYING */


#include "golf.h"

static int wall_array_length = 0;
static struct wall wall_array[6];
static struct wall WALL1;
//static struct ball ballww;
int startcount = 50; //some of these, (at least ballsize) can be defines instead, some are leftovers from labs. (textstring at least)
double ballx = 16;
double bally = 16;
double balldx, balldy;
double balldirection;
int ballinhole = 0;
double aim = PI/2; // försöka göra om aim till int
int intaim = 90;
double ballvelocity = 1;
int ballsize = 3;
int totalscore;
int currentscore;
int errno;
//uint8_t collisionmap[32][128];
int x = 1;
uint8_t charge = 0;
int chargingup;
int holex = 110;
int holey = 16;
int timeoutcount = 50;
int introtimer = 3;
char textstring[] = "text, more text, and even more text!";
static enum gamestate current_game_state = aiming;
static enum gamestate previous_game_state;
static enum menustate current_menu_state = intro;

void init_ball(void){
	ball.x = 25;
	ball.y = 16;
	ball.diameter = 1;
}

int ball_same_direction(double x, double y){ //takes a vector (x, y) as input, checks if the balls vector (balldx, balldy) is pointing in the same direction (dot product > 0)  returns 1 true, or 0 false.
	double dotproduct = ((balldx * x) + (balldy * y)) / ((x*x)+(y*y));
	if (dotproduct >= 0) return 1;
	else return 0;


}



void moveball( void ){
	//ballx+= ballvelocity * cos(balldirection);
	//bally+= ballvelocity * sin(balldirection);
	ballx+= ballvelocity * balldx;
	bally+= ballvelocity * balldy;
	ballvelocity = ballvelocity * BALL_SLOWDOWN;
}	



void set_scorecard( void ){ //Updates the scorecard text
	display_string(0, "Score: ");
	display_string(1, itoaconv(currentscore));
	//num32asc( &textbuffer[0][8], itoaconv(currentscore) );
	//set_Char (0, 4, itoaconv(currentscore));
	
	display_string(2, "Total: ");
	display_string(3, itoaconv(totalscore + currentscore));
	
	//för debugg
	display_string(2, "intaim: ");
	display_string(3, itoaconv(intaim));
	display_string(3, itoaconv((int)wall_array[0].length));
	
	//display_string(2, itoaconv(sizeof(walls) / sizeof(walls[0])));
	display_string(0, itoaconv((int) ballx));
	display_string(1, itoaconv((int) bally));
	
	display_string(2, itoaconv((int) absoluted(holex-ballx)));
	display_string(3, itoaconv((int) absoluted(holey-bally)));
	display_string(0, itoaconv(ballinhole));
}

void advance_game( void){ //advances the game 1 frame. om gamestate = aiming, ritar sikte, flyttar sikte om knapper trycks. om gamestate = charging, ändrar charge variablen + medföljande lampor
// om gamestate = moving, flyttar bollen, samt går tillbaka till aiming om bollens hastighet är låg.
// alla gamestates: ritar nästa frame på skärmen.
	int i,j;
	//display_myimage(96);
	//draw_pixel(5,5);
	//draw_pixel(5,10);
	//volatile int* leds = (volatile int*) 0xbf886110;
	//clear_display();
	int btns= getbtns(); 
	//set_map(); //nollställer displayen till mappen
	clear_screen();
	switch (current_game_state){
		case(aiming):
				if ((btns & 4 ) == 4){
					//aim+= PI/180;
					//intaim=intaim+2;
					intaim++;
					if (intaim > 359) intaim+=360;
				}
				if ((btns & 2 )== 2){
					//aim-= PI/180;
					//intaim=intaim-2;
					intaim--;
					if (intaim < 0) intaim+=360;
				}
				//draw_aim(ballx, bally, aim); //insert intaim -> radianer  *360?
				draw_aim(ballx, bally, ((double)intaim/(180/PI)));
				if ((btns & 8 ) == 8){ // 4 -> 8
					//*leds = *leds | (btns*16); // old test function, led 3 + 4 = 7 lights up
					charge = 0;
					timeoutcount = 5;
					chargingup = 1;
					current_game_state = charging;
					

				}
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
				if ((charge == 8 && chargingup > 0) || (charge == 0 && chargingup < 0)) chargingup = -chargingup;
				else{ 
					charge = charge + chargingup;
					if (chargingup < 0) reset_led(charge + 1);
					else set_led(charge);
				}
				//if (charge == 0) chargingup = 1;
/* 				if (chargingup == 1){
					charge+= 1;
					*leds = *leds | 1 <<(8-charge);
				} else{
					charge-= 1;
					*leds = *leds & (0xFF & (0xFF << (8-charge))); // charge = 6 => & 11111110  charge = 5 => && 11111100 charge = 0 7-charge 0s
				} */
				
			} else{
				ballvelocity = 0.3 + (0.2 * charge); //this should be a variable depending on charge to hit harder/softer
				charge = 0;
				balldirection = aim;
				//balldx = cos(aim);
				//balldy = sin(aim);
				//intaim
				balldx = cos(((double)intaim/(180/PI)));
				balldy = sin(((double)intaim/(180/PI)));
				reset_led_all();
				//*leds = *leds & 0xFFFFFF00;
				//next_state = moving;
				current_game_state = moving;
				currentscore++;
			}
			break;
		case(moving):
			check_collision();
			moveball();
			if (ballvelocity < MIN_BALL_SPEED){
				ballvelocity = 0;
				current_game_state = aiming;
			}
			break;
			
		//default:
			//default stuff if no state
	}
	//draw_walls(walls); // doesnt work, arrays sent as arguments become pointers or something?
	// could make global variable length to indicate number of loaded vectors instead.
	//int length = sizeof(wall_array) / sizeof(wall_array[0]);
	for (i = 0; i < wall_array_length; i++){
		draw_wall(wall_array[i]);
	}
	draw_wall(WALL1); //ritar vektorn WALL1, för testning då vektorerna bråkat
	draw_hole(holex, holey);
	set_ball((int)(ballx + 0.5), (int)(bally+0.5)); //runder upp bollkoordinater om dom är över  x.5.
	//set_ball((int)ballx, (int)bally);
	update_display();
}

//asin acos doesnt work 
/* int get_balldirection(void){
	double newballdirection = acos(balldx);
	if(asin(balldy) < 0) newballdirection = (2*PI) - newballdirection;
	//balldirection = 

	return (int)(newballdirection * 180 / PI);
} */

/* Interrupt Service Routine */
void user_isr( void )
{
	IFSCLR(0) = 0x100; // restarts timer
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
				timeoutcount--;     //This is just a delay loop so we actually show the intro screen for a noticeable amount of time, should probably be implemented differently
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
				advance_game(); //move ball + redraws everything. takes time however. 
				
				// Final game should probably just call display_update here, and in the main function have a variable 1 or 0 showing if next frame has been calculated. If 0, check collisions, move ball, draw frame to array, and here we just push the array to the screen.
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

void load_map_vector (int n){
	if (n==1){
		struct wall ws[2]; //hård kodade vektorer för test 
		struct wall w;
		w.x = 55;
		w.y = 0;
		w.direction = 45;
		w.length = 22;
		//walls[0] = w;
/* 		walls[0].x = 1;
		walls[0].y = 1;
		walls[0].direction = 0;
		walls[0].length = 67;
		
		
		walls[1].x = 55;
		walls[1].y = 8;
		walls[1].direction = 0;
		walls[1].length = 12; */
		WALL1 = w;
		//testing the array
		w.x = 30;
		w.y = 10;
		w.direction = 90;
		w.length = 10;
		ws[0] = w;
		
		w.x = 30;
		w.y = 10;
		w.direction = 0;
		w.length = 20;
		ws[1] = w;
		
		
		wall_array[0] = ws[0];
		wall_array[1] = ws[1];
		wall_array_length = 2;
		
		
		w.x = 0;
		w.y = 0;
		w.direction = 0;
		w.length = 127;
		wall_array[2] = w;
		wall_array_length++;
		
		w.x = 0;
		w.y = 31;
		w.direction = 0;
		w.length = 126;
		wall_array[3] = w;
		wall_array_length++;
		
		w.x = 0;
		w.y = 0;
		w.direction = 90;
		w.length = 31;
		wall_array[wall_array_length] = w;
		wall_array_length++;
		
		w.x = 127;
		w.y = 0;
		w.direction = 90;
		w.length = 31;
		wall_array[wall_array_length] = w;
		wall_array_length++;
		
	}

}

/* void load_map(void){
	// loads the bitmap from data, saves it in an array in display.c & a copy for collisions  
	//OLD CODE, WE USE VECTORS NOW INSTEAD. LEFT FOR NOW BECAUSE VISUALS
	//we can reuse this to maybe draw some non collision objects bitwise later
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
						//collisionmap[i][j*8+k] = z; //loads collisions
						set_map_pixel((j*8+k), i);
					}
				}
			
			}
			
		}
		
	}
	//add collision tangents to collisionmap 
} */

/* Lab-specific initialization goes here */
void labinit( void )
{
	//volatile int* leds = (volatile int*) 0xbf886100;
	//*leds = *leds & 0xFFFFFF00;
	reset_led_all();
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
	//PR2=0x1458; //60 fps, game was runninng too fast, it didnt have time to compute calculations.
	IPCSET(2) = 0x0000000C; //set priority level   har ingen aning om vad detta gör :)
	IFSCLR(0) = 0x100;
	IECSET(0) = 0x100;
	//enable
	T2CONSET  = 0x8000;
	
	
	//set_led_all();
	
	
	
	
	//load map 1
	load_map_vector(1); // loads vectors for map "1", nothing really defined yet for others, but later the plan is to load_map_vector (n); whenever we change levels.
	// this shouldnt be here when we actually implement a menu before the game.
	//load_map(); // draws the old map
	
	
	
	
	
	enable_interrupt();
		
	
  return;
}






int check_outofboundsY(void){ //not used, early test function
	if (bally > 29 & sin(balldirection) > 0 | bally < 2 & sin(balldirection) < 0) return 1;
}

void ball_bounce(int walldirection){ // channges the balls direction, input is angle of wall we collide with in radians, should probably be converted to degrees (0-360)
	double a = balldirection;
	double b = (walldirection + 90) * PI / 180; // normal till väggen
	
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
	//double newballdirection = acos(balldx);
	//if(asin(balldy) < 0) newballdirection = (2*PI)-newballdirection;
	//balldirection = get_balldirection();
	
	// bytte till steg för steg pga rounding & overflow errors eller något.
	//double x = -2*((cos(b) * cos(a) + sin(b) * sin(a)) * cos(b) - cos(a));
	//double y = -2*((cos(b) * cos(a) + sin(b) * sin(a)) * sin(b) - sin(a));
	//double x =  - (2 *  ((  (cos(b) * cos(a)) + (sin(b) * sin(a))  )   * cos(b)) - cos(a));
	//double y =  - (2 *  ((  (cos(b) * cos(a)) + (sin(b) * sin(a))  )   * sin(b)) - sin(a));
	//-  (2 *  (  cos(b) * cos(a) + sin(b) * sin(a)  )   * sin(b)) - sin(a);
	//balldirection = atan2(y, x); //arctan ger oss 
	//balldirection = balldirection + M_PI; //temp, real =  
	//balldirection += 2*( walldirection-balldirection);
}

void check_outofboundsCol(void){ // enkel out of bounds koll då vi inte har annan kollision än. Ska ändras till att flytta skärmen senare, men behöver vector kollision först.
	if (ballx > 125 & balldx > (double)0 | ballx < 1 & balldx < (double)0 )  ball_bounce(90);    //balldx = -balldx;  simplet alternativ
	if (bally > 29 & balldy > (double)0 | bally < 2 & balldy < (double)0)   ball_bounce(0);                 //balldy = -balldy;
}

int edgecollision (void){ // används ej
	int i;
	//for (i = 0; i < collisionmap.len; i++){
	//	if ball.
	//}

}


void check_hole(void){
	int dx =  absolute((int)(holex-ballx));
	int dy =  absolute((int)(holey-bally));
	if (dx < 2) {
		if (dy < 2){
			ballvelocity = 0;
			ballinhole = 1;
			set_led_all();
		}
		
		// victory celebration temp
		
	}

}

int collision_wall (struct wall w){
	double ax = ballx - w.x;
	double ay = bally - w.y;
		// vektorn a som ska projekteras på ball
		// vektorn b är wall.x & wall.y
	double scaling;
	if (w.direction % 90 != 0) scaling = 1.415;
	else scaling = 1;	
	double bx = (cos(w.direction*DEGREE_TO_RAD) * scaling * w.length); //"vektor form på väggen, cos(direction) * scaling * length
	double by = (sin(w.direction*DEGREE_TO_RAD) * scaling * w.length);
	double dotproduct = ((ax * bx) + (ay * by)) / ((bx * bx) + (by * by));
	//double dx = cos(w.direction * PI/180);
	//double dy = sin(w.direction * PI/180);
	if ((dotproduct > 0 ) & (dotproduct < 1)){ // 1 > dot product > 0 means that the ball is inside perpendicular lines drawn at both edges of the wall.
		//betyder att bollen är "inom" vektorn.
		double perpx = ax - (dotproduct  * bx);
		double perpy = ay - (dotproduct  * by);
		//kolla avståndet
		double distance = sqrt((perpx * perpx) + (perpy * perpy)); //length of the perpendicular vector from the wall to the ball is the distance of the (center) of the ball to the wall.
		if (distance < 2.5){
			// scaling the perp vector to unit length for acos and asin functions.
			//double scaledinvertedperpx = -perpx/distance, scaledinvertedperpy = -perpy/distance; // might not actually need to scale this to unit length
			//if (ball_same_direction(scaledinvertedperpx, scaledinvertedperpy) == 1) return 1; //if balls vector is poiting same directionn as the INVERSE of the perp vector, the ball is traveling towards the wall
			
			// alternate without scaling to optimze code
			if (ball_same_direction(-perpx, -perpy) == 1) return 1;
			
			
			
		/* 	// acos asin funnkar inte pga libraries, alternativ lösning istället
			double scaledinvertedperpx = -perpx/distance, scaledinvertedperpy = -perpy/distance;
			double finaldegrees, xdegrees = acos(scaledinvertedperpx), ydegrees = asin(scaledinvertedperpy);
			if (ydegrees < 0) finaldegrees = (2*PI)-xdegrees;
			finaldegrees = finaldegrees * 180/PI;
			int upperbound = finaldegrees + 89, lowerbound = finaldegrees - 89;
			if (upperbound > 359) upperbound -= 360;
			if (lowerbound < 0) lowerbound += 360;
			if ((balldirection >= lowerbound) & (balldirection <= upperbound)) ball_bounce(w.direction);
			//dags att kolla riktnningen */		
		}	
	}
	return 0;
}

void check_collision(void){ //samlings funktion för alla collision checks för att göra kodflödet mer läsbart.
	check_outofboundsCol();
	if (ballinhole == 0)  check_hole();
	if (collision_wall(WALL1)) ball_bounce(WALL1.direction);
	int i;
	for (i = 0; i < wall_array_length; i++)	{
		if (collision_wall(wall_array[i])) {
			ball_bounce(wall_array[i].direction);
			break; //if we bounce maybe we can break? for performance upgrade, means we can only bounce once per frame though.
		}
	}
	//if (bounce == 1){
		//ball_bounce();
	//	return;
	//}

}

/* This function is called repetitively from the main program */
void labwork( void )
{
	//int potentio = analogRead(A0);
	//volatile int* leds = (volatile int*) 0xbf886110;
	//static enum gamestate current_game_state = aiming;
	enum gamestate next_state; //behövs kanske inte
	int btns = getbtns(); 
	if (current_menu_state == intro) return;
	if ((btns & 1) == 1) {
		//*leds = *leds | 1;
		set_led(8);
		current_menu_state = scorecard;
		//current_game_state = scorecard;
	} else if  (current_menu_state == scorecard) {
		reset_led(8);
		current_menu_state = playing;
		//current_game_state = previous_game_state; // value is not restored
	}
	
	switch(current_game_state){
			case(aiming):
				//if (bally > 32 | bally < 0) bally = 16;
				//if (ballx > 128 | ballx < 0) ballx = 16;
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
				//check_collision();
			
			
				break;
	}	
	
	
	//prime = nextprime(prime);
	//display_string(0, itoaconv(prime));
	//display_update();
	
	
	//*asdf = *asdf + 0x01; //Tidig del övning att binärt räkna ticks på LEDs, avstängd pga felsöknings funktion nedan
}

//enum gamestate updatestate(