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
void chestTextureInit();
void initChest(TreasureChest &);
int ballChestCollision(TreasureChest &, Ball &, ALuint &);
void initCanon(Canon & );
void CanonTextureInit();
void KaBoom(Canon &, Ball &, ALuint &);
int init_sound(ALuint &, ALuint &);
void play_sound(ALuint &);
int clean_sound(ALuint &, ALuint &);
#endif
