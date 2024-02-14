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
	int length;
	//int visible;  kan skriva om en vektor ar "visible" och lagga till en if sats for collision att direkt gallra vektorer som ej ar visible. Kan kanske andras nar vektorn ritas, ifall ingen punkt ritas sa ar visible = 0, om nagon punnkt ritas sa ar visible = 1
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