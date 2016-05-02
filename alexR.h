
#ifndef _ALEX_R_H_
#define _ALEX_R_H_

#include "vector.h"
#include "gameObjects.h"
#include <iostream>
#include <cmath>



bool isLeft(Vec &, Vec &, Vec &);
void addCurve(Curve &c, GameBoard &b);

void steeringWheelMovement(SteeringWheel &);

int rectangleBallCollision(Rectangle &, Ball &);
int bumperBallCollision(Bumper &, Ball &);
int steeringWheelBallCollision(SteeringWheel &, Ball &);
void flipperBallCollision(Flipper &, Ball &);
void applyMaximumVelocity(Ball &);
#endif

