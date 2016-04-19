#include "hseid.h"
#include <X11/Xlib.h>
#include <GL/glx.h>
#include "ppm.h"

extern Ppmimage *flippers;
extern Ppmimage *flippers2;
extern GLuint flippersTexture;
extern GLuint flippersTexture2;


void flippertexture()
{
	flippers = ppm6GetImage("./images/flippers.ppm");
        flippers2 = ppm6GetImage("./images/flippers2.ppm");

        glGenTextures(1, &flippersTexture);
        glGenTextures(1, &flippersTexture2);

        int w = flippers->width;
        int h = flippers->height;

        glBindTexture(GL_TEXTURE_2D, flippersTexture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h, 0,
            GL_RGB, GL_UNSIGNED_BYTE, flippers->data);

        w = flippers2->width;
        h = flippers2->height;

        glBindTexture(GL_TEXTURE_2D, flippersTexture2);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h, 0, 
		GL_RGB, GL_UNSIGNED_BYTE, flippers2->data);
}
