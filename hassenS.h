#ifndef _HSEID_H_
#define _HSEID_h_
#include <GL/glx.h>
#include "ppm.h"
#include <cmath>
#include <iostream>
#include "gameObjects.h"
//#include "fonts.h"

struct score {
    int points;
    int rising_points;
    int balls_left;

};

void alphaTextureInit(char *, GLuint &, Ppmimage *);
void textureInit(char *, GLuint &, Ppmimage *);
void addScore(score *, int);
void initScore(score *);
void drawScore();
#endif
