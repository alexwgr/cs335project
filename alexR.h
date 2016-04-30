
#ifndef _ALEX_R_H_
#define _ALEX_R_H_

#include "vector.h"
#include "gameObjects.h"
#include <iostream>
#include <cmath>

bool isLeft(Vec &, Vec &, Vec &);

void initGameBoard(GameBoard &);
void initBumpers(GameBoard &);

void drawCircle(Circle &);
void drawRectangle(Rectangle &);

void addBumperToBoard(Bumper &, GameBoard &);
void addCurve(Curve &c, GameBoard &b);

int bumperBallCollision(Bumper &, Ball &);
int rectangleBallCollision(Rectangle &, Ball &);
void flipperBallCollision(Flipper &, Ball &);
void applyMaximumVelocity(Ball &);
#endif

