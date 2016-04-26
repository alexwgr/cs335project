#include <iostream>
#include <string.h>
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
using namespace std;
void chestTextureInit()
{
        extern Ppmimage *openChestImage;
        extern Ppmimage *closeChestImage;
        extern GLuint openChestTexture;
        extern GLuint closeChestTexture;
        openChestImage = ppm6GetImage("./images/open-chest2.ppm");
        closeChestImage = ppm6GetImage("./images/close-chest2.ppm");

        glGenTextures(1, &openChestTexture);
        glGenTextures(1, &closeChestTexture);

        int w = openChestImage->width;
        int h = openChestImage->height;

        glBindTexture(GL_TEXTURE_2D, openChestTexture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h, 0,
                        GL_RGB, GL_UNSIGNED_BYTE, openChestImage->data);

        w = closeChestImage->width;
        h = closeChestImage->height;

        glBindTexture(GL_TEXTURE_2D, closeChestTexture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h, 0, 
                        GL_RGB, GL_UNSIGNED_BYTE, closeChestImage->data);

}
void drawChest(TreasureChest &c)
{
        extern GLuint openChestTexture;
        extern GLuint closeChestTexture;
        glPushMatrix();
        glColor3d(1.0, 1.0, 1.0);
        glTranslated(c.r.pos[0], c.r.pos[1], c.r.pos[2]);
        glRotatef(c.r.angle, 0, 0, 1);
        if(c.state == 1) {
                glBindTexture(GL_TEXTURE_2D, openChestTexture);
        }
        else if(c.state == 0) {
                glBindTexture(GL_TEXTURE_2D, closeChestTexture);
        }
        glBegin(GL_QUADS);
        glVertex2d(-c.r.width, -c.r.height); glTexCoord2f(0.0f, 1.0f);
        glVertex2d(-c.r.width, c.r.height); glTexCoord2f(0.0f, 0.0f); 
        glVertex2d(c.r.width, c.r.height); glTexCoord2f(1.0f, 0.0f); 
        glVertex2d(c.r.width, -c.r.height); glTexCoord2f(1.0f, 1.0f); 
        glEnd();
        glPopMatrix();


}
void initChest(TreasureChest &chest)
{
        Rectangle *rec = &chest.r;
        rec->pos[0] = 200.0;
        rec->pos[1] = 420.0;
        rec->width = 60.0;
        rec->height = 60.0;
        rec->angle = 0.0;
}
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
                        chest.state = 1;
                }
                return 1;
        }
        else
                return 0;
}
int init_sound(ALuint &buffer, ALuint &source)
{
        //MAKE ARRAY OF FILE NAMES for when we use more sounds
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
void play_sound(ALuint &source)
{
        alSourcePlay(source);
        //usleep(200000);
}
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
