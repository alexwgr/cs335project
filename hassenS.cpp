//Hassen Seid
//Texture Maps
//Tutorals
//menus and Prompts
//Score Keeping
#include "hassenS.h"
#include "omarO.h"
#include <X11/Xlib.h>
#include <GL/glx.h>
#include "ppm.h"
#include <iostream>
#include <math.h>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>
extern "C" {
#include "fonts.h"
}

extern Ppmimage *canonImage;
extern GLuint canonTexture;
extern score Scorekeeper;
extern Ppmimage *OceanImage;
extern Ppmimage *pinballImage;
extern Ppmimage *flippers;
extern Ppmimage *flippers2;
extern GLuint OceanTexture;
extern GLuint flippersTexture;
extern GLuint flippersTexture2;
extern GLuint pinballTexture;
extern Ball ball1;
extern int xres;
extern int yres;
#define FLIPPER_LENGTH 70.0
#define FLIPPER_HEIGHT 15.0

void alphaTextureInit(char *fileTexture, GLuint &textureID, Ppmimage *fileT)
{
    //extern Ppmimage * something;
    //extern GLuint something;
    fileT = ppm6GetImage(fileTexture);

    glGenTextures(1, &textureID);
    int w = fileT->width;
    int h = fileT->height;

    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    unsigned char * alphaData = buildAlphaData(fileT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0,
	    GL_RGBA, GL_UNSIGNED_BYTE, alphaData);
    free(alphaData);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void textureInit(char *fileTexture, GLuint &textureID, Ppmimage *fileT)
{
    //extern Ppmimage * something;
    //extern GLuint something;
    fileT = ppm6GetImage(fileTexture);

    glGenTextures(1, &textureID);
    int w = fileT->width;
    int h = fileT->height;

    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0,
	    GL_RGB, GL_UNSIGNED_BYTE, fileT->data);
    glBindTexture(GL_TEXTURE_2D, 0);
}
void addScore(score *s, int add)
{
    s->points = s->points + add;
}

void initScore(score *s)
{
    s->points = 0;
    s->balls_left = 4;
}

void drawScore()
{
    Rect re;
    re.bot = yres - 20;
    re.left = 10;
    re.center = 0;
    ggprint8b(&re, 16, 0x00ff0000, "Score: %i", Scorekeeper.points); 
    ggprint8b(&re, 16, 0x00ff0000, "Ball: %i", Scorekeeper.balls_left); 
}


