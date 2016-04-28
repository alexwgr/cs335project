#ifndef _HSEID_H_
#define _HSEID_h_

#include <cmath>
#include <iostream>
#include "gameObjects.h"
//#include "fonts.h"

struct score {
    int points;
    int balls_left;

};

void initScore(score *);
void drawScore();
void OceanBackground();
void OceanTextureInit();
void flipperstexture();
void pinballTextureInit();
void drawFlipper(Flipper &f);
void drawBall();
#endif
