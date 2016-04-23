#ifndef _GAME_OBJECTS_H

#define _GAME_OBJECTS_H

#include "vector.h"

#define MAX_RECTANGLES 900

//Collision objects
struct Rectangle {
    Vec pos;
    double width, height, angle;
};

struct Flipper {
	Vec pos;
	double angle;
	double rvel;
    //direction inverted
    bool inverted;
	//rotational velocity; gets added to angle every frame
	int flipstate;
	//0 - idle, 1 - going up, 2 - going down
};

struct GameBoard {
    Rectangle rectangles[MAX_RECTANGLES];
    int num_rectangles;
};

struct Curve {
    double width;
    Vec points[3];
};

struct Ball {
	Vec pos;
	Vec vel;
	double radius;
	double mass;
};

struct TreasureChest {
    int state; //0 is closed, 1 is open
    Rectangle r;
    int HP; // 3, 2, 1
};






#endif
