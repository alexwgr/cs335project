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
using namespace std;
extern score Scorekeeper;

/*****CANON****/
void KaBoom(Canon &c, Ball &b, ALuint &source)
{
        if(rectangleBallCollision(c.r, b))
        {
                //play_sound(source);
                cout << "KABOOM!\n";
        } 

}

void drawCanon(Canon &c)
{
        extern GLuint canonTexture;
        glPushMatrix();
        glColor3d(1.0, 1.0, 1.0);
        glTranslated(c.r.pos[0], c.r.pos[1], c.r.pos[2]);
        glRotatef(c.r.angle, 0, 0, 1);
        glBindTexture(GL_TEXTURE_2D, canonTexture);

        glEnable(GL_ALPHA_TEST);
        glAlphaFunc(GL_GREATER, 0.0f);
        glColor4ub(255,255,255,255);


        glBegin(GL_QUADS);
        glVertex2d(-c.r.width, -c.r.height); glTexCoord2f(0.0f, 1.0f);
        glVertex2d(-c.r.width, c.r.height); glTexCoord2f(0.0f, 0.0f); 
        glVertex2d(c.r.width, c.r.height); glTexCoord2f(1.0f, 0.0f); 
        glVertex2d(c.r.width, -c.r.height); glTexCoord2f(1.0f, 1.0f); 
        glEnd();

        glBindTexture(GL_TEXTURE_2D,0);
        glPopMatrix();

}
void initCanon(Canon &c)
{
        Rectangle *rec = &c.r;
        rec->pos[0] = 455.0;//x pos
        rec->pos[1] = 100.0;// y pos
        rec->width = 40.0;//thickness
        rec->height = 40.0;
        rec->angle = 0.0;

}
	
/****** SOUND *****/
//function creates sound source and buffer
int init_sound(ALuint &buffer, ALuint &source)
{
        //NOTE: NEED TO MAKE ARRAY OF FILE NAMES for when we use more sounds
        alutInit(0, NULL);
        if (alGetError() != AL_NO_ERROR) {
                cout << "ERROR: alutInit()\n";
                return 0;
        }
        //Clear error state.
        alGetError();
        //
        //Setup the listener.
        //Forward and up vectors are used.
        float vec[6] = {0.0f,0.0f,1.0f, 0.0f,1.0f,0.0f};
        alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
        alListenerfv(AL_ORIENTATION, vec);
        alListenerf(AL_GAIN, 1.0f);
        //
        //Buffer holds the sound information.
        //ALuint alBuffer;
        //alBuffer = alutCreateBufferFromFile("./sounds/test.wav");
        buffer = alutCreateBufferFromFile("./sounds/test.wav");
        //
        //Source refers to the sound.
        //ALuint alSource;
        //Generate a source, and store it in a buffer.
        //alGenSources(1, &alSource);
        alGenSources(1, &source);
        //alSourcei(alSource, AL_BUFFER, alBuffer);
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
//function plays sound when called
void play_sound(ALuint &source)
{
        alSourcePlay(source);
}
//function cleans buffer and source when called
int clean_sound(ALuint &buffer, ALuint &source)
{
        //Cleanup.
        //First delete the source.
        //alDeleteSources(1, &alSource);
        alDeleteSources(1, &source);
        //Delete the buffer.
        alDeleteBuffers(1, &buffer);
        //Close out OpenAL itself.
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
/****** TREASURE CHEST ******/
//function builds Alpha channel for image with no background
unsigned char *buildAlphaData(Ppmimage *img)
{
        //add 4th component to RGB stream...
        int i;
        int a,b,c;
        unsigned char *newdata, *ptr;
        unsigned char *data = (unsigned char *)img->data;
        newdata = (unsigned char *)malloc(img->width * img->height * 4);
        ptr = newdata;
        for (i=0; i<img->width * img->height * 3; i+=3) {
                a = *(data+0);
                b = *(data+1);
                c = *(data+2);
                *(ptr+0) = a;
                *(ptr+1) = b;
                *(ptr+2) = c;
                //get largest color component...
                //*(ptr+3) = (unsigned char)((
                //		(int)*(ptr+0) +
                //		(int)*(ptr+1) +
                //		(int)*(ptr+2)) / 3);
                //d = a;
                //if (b >= a && b >= c) d = b;
                //if (c >= a && c >= b) d = c;
                //*(ptr+3) = d;
                *(ptr+3) = (a|b|c);
                ptr += 4;
                data += 3;
        }
        return newdata;
}
//function draws treasure chest object
void drawChest(TreasureChest &c)
{
        extern GLuint openChestTexture_alpha;
        extern GLuint closeChestTexture_alpha;
        glPushMatrix();
        glColor3d(1.0, 1.0, 1.0);
        glTranslated(c.r.pos[0], c.r.pos[1], c.r.pos[2]);
        glRotatef(c.r.angle, 0, 0, 1);


        if(c.state == 1) {
                glBindTexture(GL_TEXTURE_2D, openChestTexture_alpha);
        }
        else if(c.state == 0) {
                glBindTexture(GL_TEXTURE_2D, closeChestTexture_alpha);
        }

        glEnable(GL_ALPHA_TEST);
        glAlphaFunc(GL_GREATER, 0.0f);
        glColor4ub(255,255,255,255);

        glBegin(GL_QUADS);
        glVertex2d(-c.r.width, -c.r.height); glTexCoord2f(0.0f, 1.0f);
        glVertex2d(-c.r.width, c.r.height); glTexCoord2f(0.0f, 0.0f); 
        glVertex2d(c.r.width, c.r.height); glTexCoord2f(1.0f, 0.0f); 
        glVertex2d(c.r.width, -c.r.height); glTexCoord2f(1.0f, 1.0f); 
        glEnd();

        glBindTexture(GL_TEXTURE_2D, 0);
        glPopMatrix();
}
//function initalizes treasure chest object data
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
        //only play sound if velocity is over a certain amount
        if(VecMagnitude(b.vel) > 1) {
                cout << "YOU JUST GOT 500PTS!!\n";
                play_sound(source);
                if(chest.HP > 0) {
                        chest.HP--;
                }
                if(chest.HP == 0) {
                        cout << "OPEN\n";
                        chest.state = 1;
                        addScore(&Scorekeeper, 1000);
                }
                return 1;
        }
        else
                return 0;
}
