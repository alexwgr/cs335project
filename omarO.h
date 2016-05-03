//Author: Omar Oseguera
//see CPP file for roles description
#ifndef _OMAR_O_
#define _OMAR_O_

#include <iostream>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <fcntl.h>
#include "gameObjects.h"
#include <GL/glx.h>
#include <sys/stat.h>
#include "ppm.h"
#include </usr/include/AL/alut.h>
using namespace std;
//initChest initializes treasure chest object properties
void initChest(TreasureChest &);
//ballChestCollision plays sound when ball collides with chest
int ballChestCollision(TreasureChest &, Ball &, ALuint &);
//initCanon initializes canon object properties
void initCanon(Canon & );
//KaBoom function will play canon launching sounds
void KaBoom(Canon &, Ball &, ALuint &);
//init_sound creates soudn source and buffer
int init_sound(ALuint &, ALuint &);
//plays sound
void play_sound(ALuint &);
//clean up source and buffer
int clean_sound(ALuint &, ALuint &);
#endif
