#ifndef _GAME_OBJECTS_H

#define _GAME_OBJECTS_H

#include "vector.h"

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

struct Ball {
	Vec pos;
	Vec vel;
	double radius;
	double mass;
};


//Collision objects
struct Rectangle {
    Vec pos;
    double length, height;
};

#endif
