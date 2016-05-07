//Author: Hassen Seid, Omar Oseguera, Alex Rinaldi
//Purpose - Contains resources that can be used by everyone (and don't really belong to anyone)

//Description
//Includes struct definitions for all the different objects in the game (flipper, bumper)
//Also includes useful functions like drawing debug rectangles and time/counter management


#ifndef _GAME_OBJECTS_H

#define _GAME_OBJECTS_H

#include "vector.h"
#include "time.h"
#include "ppm.h"

#define MAX_RECTANGLES 900
#define MAX_PARTICLES 800

//Collision objects
struct Rectangle {
    Vec pos;
    bool collide[4]; //up, down, left, right
    double width, height, angle;

    Rectangle() { for (int i = 0; i < 4; i++) collide[i] = true; }
};
struct Shape {
    float width, height;
    float radius;
    Vec center;
};
struct Particle {
    Shape s;
    Vec velocity;
};
struct Water {
    bool waterflow;
    Vec pos;
    int n;
    Particle particle[MAX_PARTICLES];
    Water() {
        waterflow = true;//may not need this
    }
};
struct Smoke {
		Rectangle r;
		int frame;
};
struct Circle {
    Vec pos;
    double radius;

};

//Smacks the ball when you press a button
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


// A curve that is built from small rectangles
struct Curve {
    double width;
    bool collide[2]; //above, below
    int npoints;
    Vec points[3];

    Curve() { for (int i = 0; i < 4; i++) collide[i] = true; }
};

//The main character of our story
struct Ball {
	Vec pos;
	Vec vel;
	double radius;
	double mass;
    int inPlay; //1 if ball has left launch chute
    int isVisible; // 0 is hidden, 1 is showing

    Ball() {
        inPlay = 0;
        isVisible = 1;
    }
};

//A circular object that bounces the ball away
struct Bumper {
    Circle c;
    int state; // 0 up, 1 down
};

//Opens and awards points after it is hit multiple times
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

//Shoots the ball
struct Canon {
    Rectangle r;
    int state;//use to launch canon
};

//Does a little spin when the ball moves over it
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


//Holds all of the repeating objects in the game; collision rectangles, bumpers, etc.
struct GameBoard {
    Rectangle rectangles[MAX_RECTANGLES];
    Bumper bumpers[MAX_RECTANGLES];
    int num_rectangles;
    int num_bumpers;
};


//functions

unsigned char *buildAlphaData(Ppmimage *);
//used for timing
double timeDiff(struct timespec *, struct timespec *);
void timeCopy(struct timespec *, struct timespec *);

//used to set default values 
void initGameBoard(GameBoard &);
void initBumpers(GameBoard &);
void initSteeringWheel(SteeringWheel &);

//simple filled shapes, good for debugging
void drawCircle(Circle &);
void drawRectangle(Rectangle &);

//adds a bumper to the board
void addBumperToBoard(Bumper &, GameBoard &);

#endif
