#ifndef _OMAR_O_
#define _OMAR_O_

#include <iostream>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include </usr/include/AL/alut.h>
using namespace std;
int init_sound(ALuint &, ALuint &);
void play_sound(ALuint &);
int clean_sound(ALuint &, ALuint &);
#endif
