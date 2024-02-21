/* mipslab.h
   Header file for all labs.
   This file written 2015 by F Lundevall
   Some parts are original code written by Axel Isaksson

   Latest update 2015-08-28 by F Lundevall

   For copyright and licensing, see file COPYING */
#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "mipslab.h"  /* Declatations for these labs */
#include <stdio.h>
#include <math.h>   
#include "structs.h"	
#include "io.h"
#include "misc.h"
#include <errno.h>
   

#define BALL_SLOWDOWN 0.993
#define BALL_BOUNCE_SLOWDOWN 0.87
#define PI 3.14
#define MIN_BALL_SPEED 0.2
#define RAD_TO_DEGREE 180/3.14
#define DEGREE_TO_RAD  3.14/180

#define AIM_LENGTH 7 //maybe belongs in display?
#define AIM_GAP  5

// Enums -----------------
enum gamestate {aiming, charging, moving, finished};

static inline char *stringFromGamestate(enum gamestate n)
{
    static const char *strings[] = { "aiming", "charging", "moving", "finished", /* continue for rest of values */ };

    return strings[n];
}

//should be in game.h when its made
enum menustate {intro, menu, playing, scorecard}; // can add different menu states example highscore etc, to show where in hte menu you are.