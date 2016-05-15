//Author: Omar Oseguera
//Roles and Responsibilites: 
// - This file contains code for sound
// - attempting special effects 
// - attempting sprite animation and timing
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <fcntl.h>
#include "omarO.h"
#include "gameObjects.h"
#include "alexR.h"
#include "ppm.h"
#include <sys/stat.h>
#include <GL/glx.h>
#include </usr/include/AL/alut.h>
#include "hassenS.h"
extern "C" {
#include "fonts.h"
}
using namespace std;
extern score Scorekeeper;
//initSmoke sets properties for smoke sprites
//all sprites at same position to loop through each frame

void initFlag(Flag &f)
{
	Rectangle *flagSprite = &f.r;
	flagSprite->pos[0] = 450.0;
	flagSprite->pos[1] = 150.0;
	flagSprite->width = 30.0;
	flagSprite->height = 50.0;
	flagSprite->angle = 0.0;
}
void initSmoke(Smoke &s)
{
    Rectangle *smoke_sprite = &s.r;
    smoke_sprite->pos[0] = 460.0;
    smoke_sprite->pos[1] = 150.0;
    smoke_sprite->width = 30.0;
    smoke_sprite->height = 50.0;
    smoke_sprite->angle = 0.0;

}
void smokeAnimation(Smoke &s, timespec timeCurrent)
{   
						cout << s.frame << " \n";//prints frame # to console for debugging
            glPushMatrix();
            glColor3d(1.0, 1.0, 1.0);
            glTranslated(s.r.pos[0], s.r.pos[1], s.r.pos[2]);
            glRotatef(s.r.angle + 90.0, 0, 0, 1);
            glBindTexture(GL_TEXTURE_2D, smokeSpriteTexture[s.frame]);
            glEnable(GL_ALPHA_TEST);
            glAlphaFunc(GL_GREATER, 0.0f);
            glColor4ub(255,255,255,255);

            glBegin(GL_QUADS);
            glVertex2d(-s.r.width, -s.r.height); glTexCoord2f(0.0f, 1.0f);
            glVertex2d(-s.r.width, s.r.height); glTexCoord2f(0.0f, 0.0f); 
            glVertex2d(s.r.width, s.r.height); glTexCoord2f(1.0f, 0.0f); 
            glVertex2d(s.r.width, -s.r.height); glTexCoord2f(1.0f, 1.0f); 
            glEnd();

            glBindTexture(GL_TEXTURE_2D,0);
            glPopMatrix();

            //if a 20th of a second has passed
            if (timeDiff(&s.frame_timer, &timeCurrent) > 1.0/20.0) {
                //reset the timer
                timeCopy(&s.frame_timer, &timeCurrent);
                //advance to the next frame
                s.frame++;
						} 
}
//play sound when Cannon launches ball
void KaBoom(Cannon &c, Ball &b, ALuint &source)
{
    if (rectangleBallCollision(c.r, b)) {
        play_sound(source);
        //cout << "KABOOM!\n";
    } 
}
void initCannon(Cannon &c)
{
    Rectangle *rec = &c.r;
    rec->pos[0] = 460.0;
    rec->pos[1] = 100.0;
    rec->width = 40.0;
    rec->height = 40.0;
    rec->angle = 0.0;
}
/****** SOUND *****/
//function creates sound source and buffer
int init_sound(ALuint &buffer, ALuint &source)
{
    alutInit(0, NULL);
    if (alGetError() != AL_NO_ERROR) {
        cout << "ERROR: alutInit()\n";
        return 0;
    }
    //Clear error state.
    alGetError();
    //Setup the listener.
    //Forward and up vectors are used.
    float vec[6] = {0.0f,0.0f,1.0f, 0.0f,1.0f,0.0f};
    alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
    alListenerfv(AL_ORIENTATION, vec);
    alListenerf(AL_GAIN, 1.0f);
    //Buffer holds the sound information.
    buffer = alutCreateBufferFromFile("./sounds/test.wav");
    //Source refers to the sound.
    //Generate a source, and store it in a buffer.
    alGenSources(1, &source);
    alSourcei(source, AL_BUFFER, buffer);
    //Set volume and pitch to normal, no looping of sound.
    alSourcef(source, AL_GAIN, 1.0f);
    alSourcef(source, AL_PITCH, 1.0f);
    alSourcei(source, AL_LOOPING, AL_FALSE);
    if (alGetError() != AL_NO_ERROR) {
        cout << "ERROR: setting source\n";
        return 0;
    }
    return 0;
}
//function plays sound based on source input from pinball.cpp
void play_sound(ALuint &source)
{
    alSourcePlay(source);
}
//function cleans buffer and source when called
int clean_sound(ALuint &buffer, ALuint &source)
{
    //First delete the source.
    alDeleteSources(1, &source);
    //Delete the buffer.
    alDeleteBuffers(1, &buffer);
    //Get active context.
    ALCcontext *Context = alcGetCurrentContext();
    //Get device for active context.
    ALCdevice *Device = alcGetContextsDevice(Context);
    //Disable context.
    alcMakeContextCurrent(NULL);
    //Release context(s).
    alcDestroyContext(Context);
    //Close device.
    alcCloseDevice(Device);
    return 0;
}

void initChest(TreasureChest &chest)
{
    Rectangle *rec = &chest.r;
    rec->pos[0] = 100.0;
    rec->pos[1] = 420.0;
    rec->width = 40.0;
    rec->height = 40.0;
    rec->angle = 0.0;
}
//function plays sound when ball collides with chest
int ballChestCollision(TreasureChest &chest, Ball &b, ALuint &source)
{
    if (VecMagnitude(b.vel) > 1) {
        cout << "YOU JUST GOT 500PTS!!\n";
        play_sound(source);
        if (chest.HP > 0) {
            chest.HP--;
        }
        if (chest.HP == 0) {
            cout << "OPEN\n";
            chest.state = 1;
            addScore(&Scorekeeper, 1000);
        }
        return 1;
    }
    else
        return 0;
}
