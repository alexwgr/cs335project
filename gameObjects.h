#ifndef _GAME_OBJECTS_H

#define _GAME_OBJECTS_H

#include "vector.h"

#define MAX_RECTANGLES 900

//Collision objects
struct Rectangle {
    Vec pos;
    bool collide[4]; //up, down, left, right
    double width, height, angle;

    Rectangle() { for (int i = 0; i < 4; i++) collide[i] = true; }
};

struct Circle {
    Vec pos;
    double radius;

};

struct Flipper {
	Vec pos;
	double width, height, angle;
	double rvel;
    //direction inverted
    bool inverted;
	//rotational velocity; gets added to angle every frame
	int flipstate;
	//0 - idle, 1 - going up, 2 - going down
};


struct Curve {
    double width;
    bool collide[2]; //above, below
    int npoints;
    Vec points[3];

    Curve() { for (int i = 0; i < 4; i++) collide[i] = true; }
};

struct Ball {
	Vec pos;
	Vec vel;
	double radius;
	double mass;
};

struct Bumper {
    Circle c;
    int state; // 0 up, 1 down
};

struct TreasureChest {
    int state; //0 is closed, 1 is open
    Rectangle r;
    int HP; // 3, 2, 1
	TreasureChest() 
	{
		state = 0;
		HP = 3;
	}
};


struct GameBoard {
    Rectangle rectangles[MAX_RECTANGLES];
    Bumper bumpers[MAX_RECTANGLES];
    int num_rectangles;
    int num_bumpers;
};




#endif
