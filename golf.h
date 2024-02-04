/* mipslab.h
   Header file for all labs.
   This file written 2015 by F Lundevall
   Some parts are original code written by Axel Isaksson

   Latest update 2015-08-28 by F Lundevall

   For copyright and licensing, see file COPYING */

#define BALL_SLOWDOWN 0.993
#define AIM_LENGTH 7
#define AIM_GAP  5




// Enums -----------------
enum gamestate {aiming, charging, moving};

//should be in game.h when its made
enum menustate {intro, menu, playing, scorecard}; // can add different menu states example highscore etc, to show where in hte menu you are.