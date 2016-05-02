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
extern "C" {
	#include "fonts.h"
}

char buffer[256];
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


void textureInit(char *filename, GLuint &textureID, Ppmimage *file)
{
	file = ppm6GetImage(filename);
	glGenTextures(1, &textureID);

	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	

        //unsigned char *alphaData = buildAlphaData(file);	
	glTexImage2D(GL_TEXTURE_2D, 0, 3,
				file->width, file->height,
				0, GL_RGB, GL_UNSIGNED_BYTE, file->data);
}

//Loading the Image
void OceanTextureInit()
{
    	strcpy(buffer, "./images/Ocean.ppm");
    	textureInit(buffer, OceanTexture, OceanImage);
	/*OceanImage = ppm6GetImage("./images/Ocean.ppm");
	glGenTextures(1, &OceanTexture);

	glBindTexture(GL_TEXTURE_2D, OceanTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 3,
				OceanImage->width, OceanImage->height,
				0, GL_RGB, GL_UNSIGNED_BYTE, OceanImage->data);
				*/
}	


//Load Image Flipper
void flipperstexture()
{
	
        //int w = flippers->width;
        //int h = flippers->height;
    	strcpy(buffer, "./images/flippers.ppm");
	textureInit(buffer, flippersTexture, flippers);
	/*flippers = ppm6GetImage("./images/flippers.ppm");
        glGenTextures(1, &flippersTexture);

        int w = flippers->width;
        int h = flippers->height;

        glBindTexture(GL_TEXTURE_2D, flippersTexture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h, 0,
            GL_RGB, GL_UNSIGNED_BYTE, flippers->data);

*/
	/////////////////////////////////////////
	strcpy(buffer, "./images/flippers2.ppm");
	textureInit(buffer, flippersTexture2, flippers2);
/*
        flippers2 = ppm6GetImage("./images/flippers2.ppm");

        glGenTextures(1, &flippersTexture2);
        //w = flippers2->width;
        //h = flippers2->height;

        glBindTexture(GL_TEXTURE_2D, flippersTexture2);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h, 0, 
		GL_RGB, GL_UNSIGNED_BYTE, flippers2->data);
*/
}

//load pinball
void pinballTextureInit()
{
    strcpy(buffer, "./images/pinball.ppm");
    textureInit(buffer, pinballTexture, pinballImage);
/*
    pinballImage = ppm6GetImage("./images/pinball.ppm");
    glGenTextures(1, &pinballTexture);

    int w = pinballImage->width;
    int h = pinballImage->height;

    glBindTexture(GL_TEXTURE_2D, pinballTexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h, 0,
        GL_RGB, GL_UNSIGNED_BYTE, pinballImage->data);
*/
}

//Display Image
void OceanBackground()
{
	glColor3f(1.0, 1.0, 1.0);
	glBindTexture(GL_TEXTURE_2D, OceanTexture);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 1.0f); glVertex2i(0, 0);
		glTexCoord2f(0.0f, 0.0f); glVertex2i(0, yres);
		glTexCoord2f(1.0f, 0.0f); glVertex2i(xres, yres);
		glTexCoord2f(1.0f, 1.0f); glVertex2i(xres, 0);
	glEnd();


}
//Display Image 
void drawFlipper(Flipper &f)
{
    float length = f.inverted ? -FLIPPER_LENGTH : FLIPPER_LENGTH;
    float angle = f.inverted ? -f.angle : f.angle;
    glPushMatrix();
	glColor3f(1, 1, 1);
	glTranslated(f.pos[0], f.pos[1], f.pos[2]);
	glRotatef(angle, 0, 0, 1);
	//if (f.inverted) {
		glBindTexture(GL_TEXTURE_2D, flippersTexture);
	//} else {
	//    	glBindTexture(GL_TEXTURE_2D, flippersTexture2);
	//}
	//Coord
	glBegin(GL_QUADS);
	glVertex2f(0, -FLIPPER_HEIGHT); glTexCoord2f(1.0f, 0.0f); 
	glVertex2f(length, -FLIPPER_HEIGHT); glTexCoord2f(0.0f, 0.0f);
	glVertex2f(length, 0); glTexCoord2f(0.0f, 1.0f); 
	glVertex2f(0, 0); glTexCoord2f(1.0f, 1.0f); 
	glEnd();
    glPopMatrix();
}
//display pinball
void drawBall()
{
	float angle, radian, x, y, tx, ty, xcos, ysin;

	glBindTexture(GL_TEXTURE_2D, pinballTexture);
	glBegin(GL_POLYGON);
	//for (i=0; i<n; i++) {
	//	glVertex2d(verts[i][0]*rad, verts[i][1]*rad);
//	}

	for(angle = 0.0; angle < 360.0; angle+= 2.0)
	{
	    radian = angle * (M_PI/100.0f);

        xcos = (float)cos(radian);
        ysin = (float)sin(radian);
        x = xcos * ball1.radius;
        y = ysin * ball1.radius;
        tx = xcos * 0.5 + 0.5;
        ty = ysin * 0.5 + 0.5;

	    glTexCoord2f(tx, ty);
	    glVertex2f(x, y);
	}
	glEnd();
}

