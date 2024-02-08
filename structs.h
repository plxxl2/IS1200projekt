/* mipslab.h
   Header file for all labs.
   This file written 2015 by F Lundevall
   Some parts are original code written by Axel Isaksson

   Latest update 2015-08-28 by F Lundevall

   For copyright and licensing, see file COPYING */
//#ifndef STRUCTS_H
#define STRUCTS_H
   

struct wall {
	int x;
	int y;
	int direction;
	float length;

}  wall;



struct ball {
	float x;
	float y;
	float dy;
	float dx;
	int diameter;
	int direction;
	float speed;

}  ball;