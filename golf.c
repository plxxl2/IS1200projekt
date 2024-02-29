/* mipslabwork.c
asdf
   This file written 2015 by F Lundevall
   Updated 2017-04-21 by F Lundevall

   This file should be changed by YOU! So you must
   add comment(s) here with your name(s) and date(s):

   This file modified 2017-04-31 by Ture Teknolog 

   For copyright and licensing, see file COPYING */


#include "golf.h"
#include "maps.h"

static int wall_array_length = 0;
static struct wall wall_array[42];
//static struct ball ballww;
int startcount = 50; //some of these, (at least ballsize) can be defines instead, some are leftovers from labs. (textstring at least)
double ballx = 16;
double bally = 16;
double balldx, balldy;
double balldirection;
int ballinhole = 0;
double aim = PI/2; // forsoka gora om aim till int
int intaim = 90;
double ballvelocity = 1;
int ballsize = 3;
int errno;
//uint8_t collisionmap[32][128];
int x = 1;
uint8_t charge = 0;
int chargingup;
int holex = 110;
int holey = 16;
int timeoutcount = 50;
int introtimer = 0;
int waitforinputchange = 0;

static enum gamestate current_game_state = aiming;
static enum gamestate previous_game_state;
static enum menustate current_menu_state = intro;
int loadedmap = 0;
int currentmap = 1;
int parValues[5] = {2, 2, 4, 3, 3};
int score[5];

int current_shots = 0;
int tick = 0;
int tickiterator = 1;
int tickperiod = 15;

int introloaded = 0;

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
	ballx += ballvelocity * balldx;
	bally += ballvelocity * balldy;
	ballvelocity = ballvelocity * BALL_SLOWDOWN;
}	

void set_scorecard( void ){ //Updates the scorecard text
	display_string(0, itoaconv(tick));
	display_string(1, itoaconv(tickiterator));
	display_string(2, itoaconv(loadedmap));
	display_string(3, itoaconv(tickperiod));
}

void advance_game( void){ //advances the game 1 frame. om gamestate = aiming, ritar sikte, flyttar sikte om knapper trycks. om gamestate = charging, andrar charge variablen + medfoljande lampor
// om gamestate = moving, flyttar bollen, samt gar tillbaka till aiming om bollens hastighet ar lag.
// alla gamestates: ritar nasta frame pa skarmen.
	int i,j;

	int btns = getbtns(); 

	if (loadedmap && !tickperiod) {
		tickperiod = 15;
		clear_screen();
		load_map_vector(currentmap);
	} else if (loadedmap && tickperiod){
		tickperiod--;
	}
	switch (current_game_state){
		case(aiming):
				if ((btns & 4 ) == 4){
					intaim--;
					if (intaim < 0) intaim+=360;
				}
				if ((btns & 2 )== 2){

					intaim++;
					if (intaim > 359) intaim+=360;
					
				}

				update_display_ball_aim((int)(ballx + 0.5), (int)(bally + 0.5), ((double)intaim/(180/PI)));
				if ((btns & 8 ) == 8){ // 4 -> 8

					charge = 0;
					reset_led_all();
					timeoutcount = 5;
					chargingup = 1;
					current_game_state = charging;
					

				}

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
				
			} else{
				ballvelocity = 0.3 + (0.2 * charge); //this should be a variable depending on charge to hit harder/softer
				charge = 0;
				balldirection = aim;

				balldx = cos(((double)intaim/(180/PI)));
				balldy = sin(((double)intaim/(180/PI)));
				reset_led_all();

				current_game_state = moving;
				score[currentmap - 1] += 1;
			}
			update_display_ball_aim((int)(ballx + 0.5), (int)(bally + 0.5),(((double)intaim)/(180/PI)));
			current_shots++;
			break;
		case(moving):
			check_collision();
			moveball();
			if (ballvelocity < MIN_BALL_SPEED){ // Stop ball when velocity gets to a certain point
				ballvelocity = 0;
				current_game_state = aiming;
			}
			if (ballinhole) current_game_state = finished;
			update_display_ball((int)(ballx + 0.5), (int)(bally + 0.5));
			break;
		case(finished):

			if (ballinhole) {

			// Rest game state variables
			ballinhole = 0;
			ballx = 16;
			bally = 16;
			tick = 0;
			tickiterator = 1;
			tickperiod = 15;
			
			clear_screen();

		char result[50]; // Make sure this is large enough to hold the longest string

		int diff = score[currentmap - 1] - parValues[currentmap - 1]; // How much above/under par?

		if (diff >= 3) {
			strcpy(result, "Too bad..!");
		} else if (score[currentmap - 1] == 1) {
			strcpy(result, "Hole in one! Wow");
		} else {
			switch (diff) {
				case -2:
					strcpy(result, "Epic eagle!");
					break;
				case -1:
					strcpy(result, "Nice birdie!");
					break;
				case 0:
					strcpy(result, "Par! Nice job.");
					break;
				case 1:
					strcpy(result, "That's a bogey!");
					break;
				case 2:
					strcpy(result, "Double bogey..");
					break;
				default:
					break;
			}
		}

			// Create and print scoreboard

			char score1[255]; // Assuming a maximum string length of 20 characters
			strcpy(score1, "P 1: ");
			strcat(score1, itoaconv(score[0]));
			strcat(score1, " ");
			strcat(score1, itoaconv(score[1]));
			strcat(score1, " ");
			strcat(score1, itoaconv(score[2]));
			strcat(score1, " ");
			strcat(score1, itoaconv(score[3]));
			strcat(score1, " ");
			strcat(score1, itoaconv(score[4]));
			strcat(score1, " ");

			char parValuesString[255];
			strcpy(parValuesString, "Par: ");
			strcat(parValuesString, itoaconv(parValues[0]));
			strcat(parValuesString, " ");
			strcat(parValuesString, itoaconv(parValues[1]));
			strcat(parValuesString, " ");
			strcat(parValuesString, itoaconv(parValues[2]));
			strcat(parValuesString, " ");
			strcat(parValuesString, itoaconv(parValues[3]));
			strcat(parValuesString, " ");
			strcat(parValuesString, itoaconv(parValues[4]));
			strcat(parValuesString, " ");

			display_string(0, result);
			display_string(1, score1);
			display_string(2, parValuesString);
			display_string(3, "Push any button for next level!");
			display_update();

			loadedmap = 0;
			
			}


			if (btns && !loadedmap && currentmap < 5) { // Go to next level when button is pressed
				currentmap++;
				clear_screen();
				load_map_vector(currentmap);
				current_game_state = aiming;
				loadedmap = 1;
			} else if (btns && currentmap == 5) {	// Reset game after last level
				currentmap = 1;
				loadedmap = 0;
				introloaded = 0;
				score[0] = 0;
				score[1] = 0;
				score[2] = 0;
				score[3] = 0;
				score[4] = 0;
				current_menu_state = intro;
				current_game_state = aiming;
				waitforinputchange = 1;
			}


			break;
		
	}
	
}

/* Interrupt Service Routine */

void user_isr( void )
{
	IFSCLR(0) = 0x100; // restarts timer
	
	switch(current_menu_state){
		case(intro):
				
			if (!introloaded) { // Show intro screen
				introloaded = 1;
				display_update();
				display_image(introtimer, golfBall);
				display_image(0, intro1);
				display_image(32, intro2);
				display_image(64, intro3ball);
				display_image(96, intro4);
			}
			break;
		case(instructions):
				
			if (!introloaded) { // Show instructions screen
				introloaded = 1;
				display_update();
				display_image(introtimer, golfBall);
				display_image(0, inst1);
				display_image(32, inst2);
				display_image(64, inst3);
				display_image(96, inst4);
			}
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

}

void load_map_vector (int n){
	struct wall w;
	wall_array_length = 0;
	int i = 0;
	switch (n)
	{
	case 1:

		// Access and print the vectors
		for (i; i < 12; ++i) {
			w.x = map1vectors[i][0];
			w.y = map1vectors[i][1];
			w.direction = map1vectors[i][2];
			w.length = map1vectors[i][3];
			wall_array[wall_array_length] = w;
			wall_array_length++;
		}
		i = 0;

		break;
	
	case 2:
		if (tick >= 12) {
			tickiterator = -1;
		}
		if (tick <= 0) {
			tickiterator = 1;
		}

		tick += tickiterator;

		// Access and print the vectors
		for (i; i < 10; ++i) {
			w.x = map2vectors[i][0];
			w.y = map2vectors[i][1];
			w.direction = map2vectors[i][2];
			w.length = map2vectors[i][3];
			wall_array[wall_array_length] = w;
			wall_array_length++;
		}
		i = 0;

		w.x = 65;
		w.y = 0;
		w.direction = 90;
		w.length = tick;
		wall_array[wall_array_length] = w;
		wall_array_length++;

		w.x = 65;
		w.y = 32;
		w.direction = 270;
		w.length = 12 - tick;
		wall_array[wall_array_length] = w;
		wall_array_length++;

		break;

	

	case 3:
		if (tick >= 12) {
			tickiterator = -1;
		}
		if (tick <= 0) {
			tickiterator = 1;
		}

		tick += tickiterator;

		w.x = 45;
		w.y = 11;
		w.direction = 90;
		w.length = tick;
		wall_array[wall_array_length] = w;
		wall_array_length++;

		w.x = 75;
		w.y = 22;
		w.direction = -90;
		w.length = 12 - tick;
		wall_array[wall_array_length] = w;
		wall_array_length++;

		w.x = 105;
		w.y = 11;
		w.direction = 90;
		w.length = tick;
		wall_array[wall_array_length] = w;
		wall_array_length++;

		// Access and print the vectors
		for (i; i < 38; ++i) {
			w.x = map3vectors[i][0];
			w.y = map3vectors[i][1];
			w.direction = map3vectors[i][2];
			w.length = map3vectors[i][3];
			wall_array[wall_array_length] = w;
			wall_array_length++;
		}
		i = 0;

		

		break;

		case 4:
		if (tick >= 13) {
			tickiterator = -1;
		}
		if (tick <= -13) {
			tickiterator = 1;
		}

		tick += tickiterator;
		
		w.x = 25;
		w.y = 0;
		w.direction = 90;
		w.length = 13 + tick;
		wall_array[wall_array_length] = w;
		wall_array_length++;

		w.x = 25;
		w.y = 32;
		w.direction = -90;
		w.length = 13 - tick;
		wall_array[wall_array_length] = w;
		wall_array_length++;

		w.x = 40;
		w.y = 0;
		w.direction = 90;
		w.length = 13 - tick;
		wall_array[wall_array_length] = w;
		wall_array_length++;

		w.x = 40;
		w.y = 32;
		w.direction = -90;
		w.length = 13 + tick;
		wall_array[wall_array_length] = w;
		wall_array_length++;

		w.x = 55;
		w.y = 0;
		w.direction = 90;
		w.length = 13 + tick;
		wall_array[wall_array_length] = w;
		wall_array_length++;

		w.x = 55;
		w.y = 32;
		w.direction = -90;
		w.length = 13 - tick;
		wall_array[wall_array_length] = w;
		wall_array_length++;

		w.x = 70;
		w.y = 0;
		w.direction = 90;
		w.length = 13 - tick;
		wall_array[wall_array_length] = w;
		wall_array_length++;

		w.x = 70;
		w.y = 32;
		w.direction = -90;
		w.length = 13 + tick;
		wall_array[wall_array_length] = w;
		wall_array_length++;

		w.x = 85;
		w.y = 0;
		w.direction = 90;
		w.length = 13 + tick;
		wall_array[wall_array_length] = w;
		wall_array_length++;

		w.x = 85;
		w.y = 32;
		w.direction = -90;
		w.length = 13 - tick;
		wall_array[wall_array_length] = w;
		wall_array_length++;

				// Access and print the vectors
		for (i; i < 7; ++i) {
			w.x = map4vectors[i][0];
			w.y = map4vectors[i][1];
			w.direction = map4vectors[i][2];
			w.length = map4vectors[i][3];
			wall_array[wall_array_length] = w;
			wall_array_length++;
		}
		i = 0;


		break;

		case 5:

		if (tick >= 15) {
			tickiterator = -1;
		}
		if (tick <= 0) {
			tickiterator = 1;
		}

		tick += tickiterator;

		w.x = 90;
		w.y = 0;
		w.direction = 45;
		w.length = 12 + tick;
		wall_array[wall_array_length] = w;
		wall_array_length++;

						// Access and print the vectors
		for (i; i < 19; ++i) {
			w.x = map5vectors[i][0];
			w.y = map5vectors[i][1];
			w.direction = map5vectors[i][2];
			w.length = map5vectors[i][3];
			wall_array[wall_array_length] = w;
			wall_array_length++;
		}
		i = 0;



		break;
	}	

	//init the background
	draw_background();


}

/* Lab-specific initialization goes here */
void labinit( void )
{

	reset_led_all();

	TRISD = TRISD | 4064;
	TRISFSET = 1;

	TMR2 = 0;  // reset timer register
	T2CON = 0x0; // reset timer config
	T2CONSET = 0b111 << 4; // Set scaling to 1:256

	PR2=0x0C45; // 100 fps

	IPCSET(2) = 0x0000000C; //set priority level   har ingen aning om vad detta gor :)
	IFSCLR(0) = 0x100;
	IECSET(0) = 0x100;
	//enable
	T2CONSET  = 0x8000;

	enable_interrupt();
	
  return;
}

int check_outofboundsY(void){ //not used, early test function
	if (bally > 29 & sin(balldirection) > 0 | bally < 2 & sin(balldirection) < 0) return 1;
}

void ball_bounce(int walldirection){ // channges the balls direction, input is angle of wall we collide with in radians, should probably be converted to degrees (0-360)
	double a = balldirection;
	double b = (walldirection + 90) * PI / 180; // normal till vaggen
	

	double ax = balldx;
	double ay = balldy;
	double bx = cos(b);
	double by = sin(b);

	double nv = (bx * ax) + (by * ay); //punktprodukt av a ' b
	
	double x = ax - (2*nv*bx);
	double y = ay - (2*nv*by);
	balldx = x;
	balldy = y;
	ballvelocity = ballvelocity * BALL_BOUNCE_SLOWDOWN;
}

void check_outofboundsCol(void){ // enkel out of bounds koll da vi inte har annan kollision an. Ska andras till att flytta skarmen senare, men behover vector kollision forst.
	if (ballx > 125 & balldx > (double)0 | ballx < 1 & balldx < (double)0 )  ball_bounce(90);    //balldx = -balldx;  simplet alternativ
	if (bally > 29 & balldy > (double)0 | bally < 2 & balldy < (double)0)   ball_bounce(0);                 //balldy = -balldy;
}

void check_hole(){

	int dx =  absolute((int)(holex-ballx));
	int dy =  absolute((int)(holey-bally));
	if (dx < 2) {
		if (dy < 2){ // If delta x and delta y are less than 2, the hole was hit
			ballvelocity = 0;
			ballinhole = 1;
			set_led_all();
		}
		
	}
}

void draw_background(){
	clear_screen();
	int i;
	for (i = 0; i < wall_array_length; i++){
		draw_wall(wall_array[i]);
	}
	draw_hole(holex, holey);
}

int collision_wall (struct wall w){
	double ax = ballx - w.x;
	double ay = bally - w.y;
		// vektorn a som ska projekteras pa ball
		// vektorn b ar wall.x & wall.y
	double scaling;
	if (w.direction % 90 != 0) scaling = 1.415;
	else scaling = 1;	
	double bx = (cos(w.direction*DEGREE_TO_RAD) * scaling * w.length); //"vektor form pa vaggen, cos(direction) * scaling * length
	double by = (sin(w.direction*DEGREE_TO_RAD) * scaling * w.length);
	double dotproduct = ((ax * bx) + (ay * by)) / ((bx * bx) + (by * by));

	if ((dotproduct > 0 ) & (dotproduct < 1)){ // 1 > dot product > 0 means that the ball is inside perpendicular lines drawn at both edges of the wall.
		//betyder att bollen ar "inom" vektorn.
		double perpx = ax - (dotproduct  * bx);
		double perpy = ay - (dotproduct  * by);
		//kolla avstandet
		double distance = sqrt((perpx * perpx) + (perpy * perpy)); //length of the perpendicular vector from the wall to the ball is the distance of the (center) of the ball to the wall.
		if (distance < 2.5){
			// scaling the perp vector to unit length for acos and asin functions.

			if (ball_same_direction(-perpx, -perpy) == 1) return 1;
			

		}	
	}
	return 0;
}

void check_collision(void){ //samlings funktion for alla collision checks for att gora kodflodet mer lasbart.
	check_outofboundsCol();
	if (ballinhole == 0)  check_hole();
	int i;
	for (i = 0; i < wall_array_length; i++)	{
		if (collision_wall(wall_array[i])) {
			ball_bounce(wall_array[i].direction);
			break; //if we bounce maybe we can break? for performance upgrade, means we can only bounce once per frame though.
		}
	}


}

/* This function is called repetitively from the main program */
void labwork( void )
{
	enum gamestate next_state; //behovs kanske inte

	int btns = getbtns(); 
	if (current_menu_state == intro) {
		if (btns) {
			if (waitforinputchange) return;
			current_menu_state = instructions;
			if (!loadedmap) {
				introloaded = 0;
				}
		}
		if (!btns) waitforinputchange = 0;

	}
	if (current_menu_state == instructions) {
		if ((btns & 1) == 1) {
			current_menu_state = playing;
			if (!loadedmap) {
					//load map 1
					loadedmap = 1;
					load_map_vector(currentmap); // loads vectors for map "1", nothing really defined yet for others, but later the plan is to load_map_vector (n); whenever we change levels.
				}
		}

	}

	}
