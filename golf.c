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
static struct wall wall_array[36];
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
	//ballx+= ballvelocity * cos(balldirection);
	//bally+= ballvelocity * sin(balldirection);
	ballx+= ballvelocity * balldx;
	bally+= ballvelocity * balldy;
	ballvelocity = ballvelocity * BALL_SLOWDOWN;
}	

void set_scorecard( void ){ //Updates the scorecard text
	display_string(0, itoaconv(tick));
	display_string(1, itoaconv(tickiterator));
	display_string(2, itoaconv(loadedmap));
	display_string(3, itoaconv(tickperiod));
	//num32asc( &textbuffer[0][8], itoaconv(currentscore) );
	//set_Char (0, 4, itoaconv(currentscore));
	
	// display_string(2, "Total: ");
	// display_string(3, itoaconv(totalscore + currentscore));
	
	//for debugg
	// display_string(2, "intaim: ");
	// display_string(3, itoaconv(intaim));
	// display_string(3, itoaconv((int)wall_array[0].length));
	
	//display_string(2, itoaconv(sizeof(walls) / sizeof(walls[0])));
	// display_string(0, itoaconv((int) ballx));
	// display_string(1, itoaconv((int) bally));
	
	// display_string(2, itoaconv((int) absoluted(holex-ballx)));
	// display_string(3, itoaconv((int) absoluted(holey-bally)));
	//display_string(3, itoaconv(ballinhole));
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
			if (ballvelocity < MIN_BALL_SPEED){
				ballvelocity = 0;
				current_game_state = aiming;
			}
			if (ballinhole) current_game_state = finished;
			update_display_ball((int)(ballx + 0.5), (int)(bally + 0.5));
			break;
		case(finished):
			// Improvements: run this only once
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

		int diff = score[currentmap - 1] - parValues[currentmap - 1];

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


			if (btns && !loadedmap && currentmap < 5) {
				currentmap++;
				clear_screen();
				load_map_vector(currentmap);
				current_game_state = aiming;
				loadedmap = 1;
			} else if (btns && currentmap == 5) {
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

void redraw_background (void){ // exempel pa hur vi skulle redrawa background for ny level t.ex.
	// step 1: ladda in nya vektorer i WALL_ARRAY
	int i, antal_vaggar; //antal_vaggar hamtas fran data samling, maste inkludera hur manga vaggar ar i nya leveln
	//antal_vaggar =     //hamta fran datan nagonstans
	for (i = 0; i < antal_vaggar; i++){ // for loop for att hamta nya vektorerna, peka dom mot hur du nu lagt upp datan.
	//	wall_array[i].x = 
	//	wall_array[i].y =
	//	wall_array[i].direction =
	//	wall_array[i].length = 
	}
	wall_array_length = antal_vaggar; //sa vi har den variablen
	clear_screen(); //satter background arrayen till 0 overallt
	for (i = 0; i < wall_array_length; i++){
		draw_wall(wall_array[i]);
	}
	//endast 1 av nedanstaende nar vi ska skriva till skarmen, gors under spelets gang, behovs ej har annat an for test.
	update_display(); //laddar upp arrayen pa skarmen
	update_display_ball(); //laddar upp arrayen pa skarmen samt injicerar bollen.
	update_display_ball_aim(); //laddar upp arrayen pa skarmen samt injicerar bollen & aim linjen
	// for att effektivt rita in flyttbara objekt sa vill vi formodligen injicera dom linkande hur aim linjen fungerar, kopiera den funktionen och lagg till det. Behovs aven en funktion som ar en kopia av update_display_ball som bara injicerar flyttbara objekten + bollen, utan aim linjen for de frames da bollen ror sig, da ska ingen aim linje synas.
}

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
				
			if (!introloaded) {
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
				
			if (!introloaded) {
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
	switch (n)
	{
	case 1:


		w.x = 0;
		w.y = 0;
		w.direction = 0;
		w.length = 128;
		wall_array[wall_array_length] = w;
		wall_array_length++;

		w.x = 0;
		w.y = 31;
		w.direction = 0;
		w.length = 128;
		wall_array[wall_array_length] = w;
		wall_array_length++;
		
		w.x = 0;
		w.y = 0;
		w.direction = 90;
		w.length = 32;
		wall_array[wall_array_length] = w;
		wall_array_length++;
		
		w.x = 127;
		w.y = 0;
		w.direction = 90;
		w.length = 32;
		wall_array[wall_array_length] = w;
		wall_array_length++;
		
		w.x = 50;
		w.y = 22;
		w.direction = 90;
		w.length = 10;
		wall_array[wall_array_length] = w;
		wall_array_length++;

		w.x = 50;
		w.y = 0;
		w.direction = 90;
		w.length = 10;
		wall_array[wall_array_length] = w;
		wall_array_length++;

		w.x = 60;
		w.y = 16;
		w.direction = 45;
		w.length = 6;
		wall_array[wall_array_length] = w;
		wall_array_length++;

		w.x = 60;
		w.y = 16;
		w.direction = -45;
		w.length = 6;
		wall_array[wall_array_length] = w;
		wall_array_length++;

		w.x = 70;
		w.y = 16;
		w.direction = -135;
		w.length = 6;
		wall_array[wall_array_length] = w;
		wall_array_length++;

		w.x = 70;
		w.y = 16;
		w.direction = 135;
		w.length = 6;
		wall_array[wall_array_length] = w;
		wall_array_length++;
		break;
	
	case 2:
		if (tick >= 12) {
			tickiterator = -1;
		}
		if (tick <= 0) {
			tickiterator = 1;
		}

		tick += tickiterator;


		w.x = 0;
		w.y = 0;
		w.direction = 0;
		w.length = 128;
		wall_array[wall_array_length] = w;
		wall_array_length++;

		w.x = 0;
		w.y = 31;
		w.direction = 0;
		w.length = 128;
		wall_array[wall_array_length] = w;
		wall_array_length++;
		
		w.x = 0;
		w.y = 0;
		w.direction = 90;
		w.length = 32;
		wall_array[wall_array_length] = w;
		wall_array_length++;
		
		w.x = 127;
		w.y = 0;
		w.direction = 90;
		w.length = 32;
		wall_array[wall_array_length] = w;
		wall_array_length++;
		
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

		w.x = 60;
		w.y = 16;
		w.direction = 45;
		w.length = 6;
		wall_array[wall_array_length] = w;
		wall_array_length++;

		w.x = 60;
		w.y = 16;
		w.direction = -45;
		w.length = 6;
		wall_array[wall_array_length] = w;
		wall_array_length++;

		w.x = 70;
		w.y = 16;
		w.direction = -135;
		w.length = 6;
		wall_array[wall_array_length] = w;
		wall_array_length++;

		w.x = 70;
		w.y = 16;
		w.direction = 135;
		w.length = 6;
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


		w.x = 0;
		w.y = 0;
		w.direction = 0;
		w.length = 128;
		wall_array[wall_array_length] = w;
		wall_array_length++;

		w.x = 0;
		w.y = 31;
		w.direction = 0;
		w.length = 128;
		wall_array[wall_array_length] = w;
		wall_array_length++;
		
		w.x = 0;
		w.y = 0;
		w.direction = 90;
		w.length = 32;
		wall_array[wall_array_length] = w;
		wall_array_length++;
		
		w.x = 127;
		w.y = 0;
		w.direction = 90;
		w.length = 32;
		wall_array[wall_array_length] = w;
		wall_array_length++;
		
		w.x = 5;
		w.y = 0;
		w.direction = 45;
		w.length = 11;
		wall_array[wall_array_length] = w;
		wall_array_length++;

		w.x = 25;
		w.y = 0;
		w.direction = 135;
		w.length = 11;
		wall_array[wall_array_length] = w;
		wall_array_length++;

		w.x = 5;
		w.y = 32;
		w.direction = -45;
		w.length = 11;
		wall_array[wall_array_length] = w;
		wall_array_length++;

		w.x = 25;
		w.y = 32;
		w.direction = -135;
		w.length = 11;
		wall_array[wall_array_length] = w;
		wall_array_length++;

		w.x = 45;
		w.y = 11;
		w.direction = 90;
		w.length = tick;
		wall_array[wall_array_length] = w;
		wall_array_length++;

		w.x = 25;
		w.y = 16;
		w.direction = 45;
		w.length = 6;
		wall_array[wall_array_length] = w;
		wall_array_length++;

		w.x = 25;
		w.y = 16;
		w.direction = -45;
		w.length = 6;
		wall_array[wall_array_length] = w;
		wall_array_length++;

		w.x = 35;
		w.y = 16;
		w.direction = -135;
		w.length = 6;
		wall_array[wall_array_length] = w;
		wall_array_length++;

		w.x = 35;
		w.y = 16;
		w.direction = 135;
		w.length = 6;
		wall_array[wall_array_length] = w;
		wall_array_length++;

		w.x = 75;
		w.y = 22;
		w.direction = -90;
		w.length = 12 - tick;
		wall_array[wall_array_length] = w;
		wall_array_length++;

		w.x = 35;
		w.y = 0;
		w.direction = 45;
		w.length = 11;
		wall_array[wall_array_length] = w;
		wall_array_length++;

		w.x = 55;
		w.y = 0;
		w.direction = 135;
		w.length = 11;
		wall_array[wall_array_length] = w;
		wall_array_length++;

		w.x = 35;
		w.y = 32;
		w.direction = -45;
		w.length = 11;
		wall_array[wall_array_length] = w;
		wall_array_length++;

		w.x = 55;
		w.y = 32;
		w.direction = -135;
		w.length = 11;
		wall_array[wall_array_length] = w;
		wall_array_length++;

		w.x = 105;
		w.y = 11;
		w.direction = 90;
		w.length = tick;
		wall_array[wall_array_length] = w;
		wall_array_length++;

		w.x = 55;
		w.y = 16;
		w.direction = 45;
		w.length = 6;
		wall_array[wall_array_length] = w;
		wall_array_length++;

		w.x = 55;
		w.y = 16;
		w.direction = -45;
		w.length = 6;
		wall_array[wall_array_length] = w;
		wall_array_length++;

		w.x = 65;
		w.y = 16;
		w.direction = -135;
		w.length = 6;
		wall_array[wall_array_length] = w;
		wall_array_length++;

		w.x = 65;
		w.y = 16;
		w.direction = 135;
		w.length = 6;
		wall_array[wall_array_length] = w;
		wall_array_length++;

		w.x = 65;
		w.y = 0;
		w.direction = 45;
		w.length = 11;
		wall_array[wall_array_length] = w;
		wall_array_length++;

		w.x = 85;
		w.y = 0;
		w.direction = 135;
		w.length = 11;
		wall_array[wall_array_length] = w;
		wall_array_length++;

		w.x = 65;
		w.y = 32;
		w.direction = -45;
		w.length = 11;
		wall_array[wall_array_length] = w;
		wall_array_length++;

		w.x = 85;
		w.y = 32;
		w.direction = -135;
		w.length = 11;
		wall_array[wall_array_length] = w;
		wall_array_length++;

		w.x = 85;
		w.y = 16;
		w.direction = 45;
		w.length = 6;
		wall_array[wall_array_length] = w;
		wall_array_length++;

		w.x = 85;
		w.y = 16;
		w.direction = -45;
		w.length = 6;
		wall_array[wall_array_length] = w;
		wall_array_length++;

		w.x = 95;
		w.y = 16;
		w.direction = -135;
		w.length = 6;
		wall_array[wall_array_length] = w;
		wall_array_length++;

		w.x = 95;
		w.y = 16;
		w.direction = 135;
		w.length = 6;
		wall_array[wall_array_length] = w;
		wall_array_length++;

		w.x = 95;
		w.y = 0;
		w.direction = 45;
		w.length = 11;
		wall_array[wall_array_length] = w;
		wall_array_length++;

		w.x = 115;
		w.y = 0;
		w.direction = 135;
		w.length = 11;
		wall_array[wall_array_length] = w;
		wall_array_length++;

		w.x = 95;
		w.y = 32;
		w.direction = -45;
		w.length = 11;
		wall_array[wall_array_length] = w;
		wall_array_length++;

		w.x = 115;
		w.y = 32;
		w.direction = -135;
		w.length = 11;
		wall_array[wall_array_length] = w;
		wall_array_length++;

		

		break;

		case 4:
		if (tick >= 13) {
			tickiterator = -1;
		}
		if (tick <= -13) {
			tickiterator = 1;
		}

		tick += tickiterator;


		w.x = 0;
		w.y = 0;
		w.direction = 0;
		w.length = 128;
		wall_array[wall_array_length] = w;
		wall_array_length++;

		w.x = 0;
		w.y = 31;
		w.direction = 0;
		w.length = 128;
		wall_array[wall_array_length] = w;
		wall_array_length++;
		
		w.x = 0;
		w.y = 0;
		w.direction = 90;
		w.length = 32;
		wall_array[wall_array_length] = w;
		wall_array_length++;
		
		w.x = 127;
		w.y = 0;
		w.direction = 90;
		w.length = 32;
		wall_array[wall_array_length] = w;
		wall_array_length++;
		
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

		w.x = 95;
		w.y = 16;
		w.direction = 45;
		w.length = 5;
		wall_array[wall_array_length] = w;
		wall_array_length++;

		w.x = 95;
		w.y = 16;
		w.direction = -45;
		w.length = 5;
		wall_array[wall_array_length] = w;
		wall_array_length++;

		break;

		case 5:

		if (tick >= 15) {
			tickiterator = -1;
		}
		if (tick <= 0) {
			tickiterator = 1;
		}

		tick += tickiterator;

		w.x = 0;
		w.y = 0;
		w.direction = 0;
		w.length = 128;
		wall_array[wall_array_length] = w;
		wall_array_length++;

		w.x = 0;
		w.y = 31;
		w.direction = 0;
		w.length = 128;
		wall_array[wall_array_length] = w;
		wall_array_length++;
		
		w.x = 0;
		w.y = 0;
		w.direction = 90;
		w.length = 32;
		wall_array[wall_array_length] = w;
		wall_array_length++;
		
		w.x = 127;
		w.y = 0;
		w.direction = 90;
		w.length = 32;
		wall_array[wall_array_length] = w;
		wall_array_length++;
		
		w.x = 30;
		w.y = 0;
		w.direction = 90;
		w.length = 13;
		wall_array[wall_array_length] = w;
		wall_array_length++;

		w.x = 30;
		w.y = 12;
		w.direction = 45;
		w.length = 10;
		wall_array[wall_array_length] = w;
		wall_array_length++;

		w.x = 0;
		w.y = 24;
		w.direction = 0;
		w.length = 28;
		wall_array[wall_array_length] = w;
		wall_array_length++;

		w.x = 35;
		w.y = 32;
		w.direction = -135;
		w.length = 8;
		wall_array[wall_array_length] = w;
		wall_array_length++;

		w.x = 45;
		w.y = 32;
		w.direction = -45;
		w.length = 8;
		wall_array[wall_array_length] = w;
		wall_array_length++;

		w.x = 50;
		w.y = 0;
		w.direction = 135;
		w.length = 12;
		wall_array[wall_array_length] = w;
		wall_array_length++;

		w.x = 39;
		w.y = 11;
		w.direction = 90;
		w.length = 11;
		wall_array[wall_array_length] = w;
		wall_array_length++;

		w.x = 60;
		w.y = 0;
		w.direction = 45;
		w.length = 12;
		wall_array[wall_array_length] = w;
		wall_array_length++;

		w.x = 71;
		w.y = 11;
		w.direction = 90;
		w.length = 11;
		wall_array[wall_array_length] = w;
		wall_array_length++;

		w.x = 53;
		w.y = 23;
		w.direction = 0;
		w.length = 5;
		wall_array[wall_array_length] = w;
		wall_array_length++;

		w.x = 65;
		w.y = 32;
		w.direction = -135;
		w.length = 8;
		wall_array[wall_array_length] = w;
		wall_array_length++;

		w.x = 76;
		w.y = 32;
		w.direction = -45;
		w.length = 8;
		wall_array[wall_array_length] = w;
		wall_array_length++;

		w.x = 82;
		w.y = 0;
		w.direction = 135;
		w.length = 12;
		wall_array[wall_array_length] = w;
		wall_array_length++;

		w.x = 90;
		w.y = 0;
		w.direction = 45;
		w.length = 12 + tick;
		wall_array[wall_array_length] = w;
		wall_array_length++;

		w.x = 96;
		w.y = 32;
		w.direction = -135;
		w.length = 8;
		wall_array[wall_array_length] = w;
		wall_array_length++;

		w.x = 84;
		w.y = 23;
		w.direction = 0;
		w.length = 5;
		wall_array[wall_array_length] = w;
		wall_array_length++;



		break;
		// w.x = 60;
		// w.y = 16;
		// w.direction = 45;
		// w.length = 5;
		// wall_array[wall_array_length] = w;
		// wall_array_length++;

		// w.x = 60;
		// w.y = 16;
		// w.direction = -45;
		// w.length = 5;
		// wall_array[wall_array_length] = w;
		// wall_array_length++;

		// w.x = 70;
		// w.y = 16;
		// w.direction = -135;
		// w.length = 5;
		// wall_array[wall_array_length] = w;
		// wall_array_length++;

		// w.x = 70;
		// w.y = 16;
		// w.direction = 135;
		// w.length = 5;
		// wall_array[wall_array_length] = w;
		// wall_array_length++;
		// break;

	}	

	//init the background
	draw_background();


}

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
	IPCSET(2) = 0x0000000C; //set priority level   har ingen aning om vad detta gor :)
	IFSCLR(0) = 0x100;
	IECSET(0) = 0x100;
	//enable
	T2CONSET  = 0x8000;
	
	//set_led_all();
	
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
	double b = (walldirection + 90) * PI / 180; // normal till vaggen
	
	//double ax = cos(a);
	//double ay = sin(a); // vi skapar 2 vektorer, bollens riktnning a, samt vaggens normal b
	double ax = balldx;
	double ay = balldy;
	double bx = cos(b);
	double by = sin(b);

	double nv = (bx * ax) + (by * ay); //punktprodukt av a ' b
	
	//double x = -(2*nv*bx) + ax;  // skapar vektorn (a,y) som ar reflektionen till a genom b
	//double y = -(2*nv*by) + ay;
	
	double x = ax - (2*nv*bx);
	double y = ay - (2*nv*by);
	balldx = x;
	balldy = y;
	ballvelocity = ballvelocity * BALL_BOUNCE_SLOWDOWN;
	//double newballdirection = acos(balldx);
	//if(asin(balldy) < 0) newballdirection = (2*PI)-newballdirection;
	//balldirection = get_balldirection();
	
	// bytte till steg for steg pga rounding & overflow errors eller nagot.
	//double x = -2*((cos(b) * cos(a) + sin(b) * sin(a)) * cos(b) - cos(a));
	//double y = -2*((cos(b) * cos(a) + sin(b) * sin(a)) * sin(b) - sin(a));
	//double x =  - (2 *  ((  (cos(b) * cos(a)) + (sin(b) * sin(a))  )   * cos(b)) - cos(a));
	//double y =  - (2 *  ((  (cos(b) * cos(a)) + (sin(b) * sin(a))  )   * sin(b)) - sin(a));
	//-  (2 *  (  cos(b) * cos(a) + sin(b) * sin(a)  )   * sin(b)) - sin(a);
	//balldirection = atan2(y, x); //arctan ger oss 
	//balldirection = balldirection + M_PI; //temp, real =  
	//balldirection += 2*( walldirection-balldirection);
}

void check_outofboundsCol(void){ // enkel out of bounds koll da vi inte har annan kollision an. Ska andras till att flytta skarmen senare, men behover vector kollision forst.
	if (ballx > 125 & balldx > (double)0 | ballx < 1 & balldx < (double)0 )  ball_bounce(90);    //balldx = -balldx;  simplet alternativ
	if (bally > 29 & balldy > (double)0 | bally < 2 & balldy < (double)0)   ball_bounce(0);                 //balldy = -balldy;
}

void check_hole(){
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
	//double dx = cos(w.direction * PI/180);
	//double dy = sin(w.direction * PI/180);
	if ((dotproduct > 0 ) & (dotproduct < 1)){ // 1 > dot product > 0 means that the ball is inside perpendicular lines drawn at both edges of the wall.
		//betyder att bollen ar "inom" vektorn.
		double perpx = ax - (dotproduct  * bx);
		double perpy = ay - (dotproduct  * by);
		//kolla avstandet
		double distance = sqrt((perpx * perpx) + (perpy * perpy)); //length of the perpendicular vector from the wall to the ball is the distance of the (center) of the ball to the wall.
		if (distance < 2.5){
			// scaling the perp vector to unit length for acos and asin functions.
			//double scaledinvertedperpx = -perpx/distance, scaledinvertedperpy = -perpy/distance; // might not actually need to scale this to unit length
			//if (ball_same_direction(scaledinvertedperpx, scaledinvertedperpy) == 1) return 1; //if balls vector is poiting same directionn as the INVERSE of the perp vector, the ball is traveling towards the wall
			
			// alternate without scaling to optimze code
			if (ball_same_direction(-perpx, -perpy) == 1) return 1;
			
			
			
		/* 	// acos asin funnkar inte pga libraries, alternativ losning istallet
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
	// if ((btns & 1) == 1) {
	// 	//*leds = *leds | 1;
	// 	set_led(8);
	// 	current_menu_state = scorecard;
	// 	//current_game_state = scorecard;
	// } else if  (current_menu_state == scorecard) {
	// 	reset_led(8);
	// 	draw_background(); //RESETS BACKGROUND, TAKES A SMALL AMOUNT OF TIME, TEMPORARY FIX TO RESET ARTIFACTS FROM AIM.
	// 	current_menu_state = playing;
	// 	//current_game_state = previous_game_state; // value is not restored
	// }
	
	switch(current_game_state){
			case(aiming):
				//if (bally > 32 | bally < 0) bally = 16;
				//if (ballx > 128 | ballx < 0) ballx = 16;
				// Positon i koden; da addition pa mytime kan ga out of bounds av klockans bas 60, och det hanteras av tick(), sa vill vi att tick() kallas mellan denna addition & displayupdate
				
				
				
				//moved to advance_game
				/* if ((btns & 8 ) == 8){ // 4 -> 8
					//*leds = *leds | (btns*16); // old test function, led 3 + 4 = 7 lights up
					charge = 0;
					reset_led_all();
					timeoutcount = 5;
					chargingup = 1;
					next_state = charging;
					current_game_state = next_state;
					

				}else{

				} */
				
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
	
	
	//*asdf = *asdf + 0x01; //Tidig del ovning att binart rakna ticks pa LEDs, avstangd pga felsoknings funktion nedan
}

//enum gamestate updatestate(