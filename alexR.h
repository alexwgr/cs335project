
#ifndef _ALEX_R_H_
#define _ALEX_R_H_

#include "vector.h"
#include "gameObjects.h"
#include <iostream>
#include <cmath>


double timeDiff(struct timespec *, struct timespec *);
void timeCopy(struct timespec *, struct timespec *);

bool isLeft(Vec &, Vec &, Vec &);

void initGameBoard(GameBoard &);
void initBumpers(GameBoard &);
void initSteeringWheel(SteeringWheel &);

void drawCircle(Circle &);
void drawRectangle(Rectangle &);

void addBumperToBoard(Bumper &, GameBoard &);
void addCurve(Curve &c, GameBoard &b);

void steeringWheelMovement(SteeringWheel &);

int rectangleBallCollision(Rectangle &, Ball &);
int bumperBallCollision(Bumper &, Ball &);
int steeringWheelBallCollision(SteeringWheel &, Ball &);
void flipperBallCollision(Flipper &, Ball &);
void applyMaximumVelocity(Ball &);
#endif

