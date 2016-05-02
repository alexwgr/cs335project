#ifndef _GAME_OBJECTS_H

#define _GAME_OBJECTS_H

#include "vector.h"
#include "time.h"

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
    int inPlay; //1 if ball has left launch chute
};

struct Bumper {
    Circle c;
    int state; // 0 up, 1 down
};

struct TreasureChest {
    int state; //0 is closed, 1 is open
    int active; // 0 if recently collided, 1 if not
    Rectangle r;
    int HP; // 3, 2, 1
    timespec collision_time;
	TreasureChest() {
        active = 1;
		state = 0;
		HP = 3;
	}
};
struct Canon {
    Rectangle r;
    int state;//use to launch canon
};

struct SteeringWheel {
    Vec pos;
    double inner_radius, outer_radius;
    double angle, rvel;
    int state; // 0 is not spinning, 1 is spinning
    timespec collision_time; //keeps track of how long the ball spins
    SteeringWheel() {
        state = 0;
        angle = 0;
        rvel = 0;
    }
};


struct GameBoard {
    Rectangle rectangles[MAX_RECTANGLES];
    Bumper bumpers[MAX_RECTANGLES];
    int num_rectangles;
    int num_bumpers;
};




#endif
