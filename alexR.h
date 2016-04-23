
#ifndef _ALEX_R_H_
#define _ALEX_R_H_

#include "vector.h"
#include "gameObjects.h"
#include <iostream>
#include <cmath>

bool isLeft(Vec &, Vec &, Vec &);
void initGameBoard(GameBoard &);
void drawRectangle(Rectangle &);
void addCurve(Curve &c, GameBoard &b);
int rectangleBallCollision(Rectangle &, Ball &);
void applyMaximumVelocity(Ball &);
#endif

