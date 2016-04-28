//cs335 Sample
//NOTE: This program contains to flippers for pinball game

//This program demonstrates billiard ball collisions
//
//program: bump.cpp
//author:  Gordon Griesel
//date:    2014
//
//-------------------------------------------
//Press arrow keys to move a ball
//Press S to slow the ball movement
//Grab and move a ball with mouse left button
//-------------------------------------------
//
//
//Depending on your Linux distribution,
//may have to install these packages:
// libx11-dev
// libglew1.6
// libglew1.6-dev
//
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
//#include <unistd.h>
#include <time.h>
#include <math.h>
#include <X11/Xlib.h>
//#include <X11/Xutil.h>
#include <X11/keysym.h>
//#include <GL/gl.h>
#include <GL/glx.h>
//#include <GL/glu.h>
//#include "log.h"



extern "C" {
#include "fonts.h"
}
#include "ppm.h"
#include "vector.h"
#include "gameObjects.h"
#include "alexR.h"
#include "omarO.h"
#include "hseid.h"

const int NUM_IMAGES = 6;

const double FLIPPER_LENGTH = 70.0;
const double FLIPPER_HEIGHT = 15.0;
const double FLIPPER_SPEED = 14.8;
const double FLIPPER_REST_ANGLE = -50;

typedef double Flt;
//typedef Flt Vec[3];
#define MakeVector(x,y,z,v) (v)[0]=(x),(v)[1]=(y),(v)[2]=(z)
#define VecNegate(a) (a)[0]=(-(a)[0]); (a)[1]=(-(a)[1]); (a)[2]=(-(a)[2]);
//#define VecDot(a,b) ((a)[0]*(b)[0]+(a)[1]*(b)[1]+(a)[2]*(b)[2])
#define VecCopy(a,b) (b)[0]=(a)[0];(b)[1]=(a)[1];(b)[2]=(a)[2];



//X Windows variables
Display *dpy;
Window win;

void initXWindows(void);
void initOpengl(void);
void initBalls(void);
void initFlipper(Flipper &, float, float, bool);
void cleanupXWindows(void);
void checkResize(XEvent *e);
void checkMouse(XEvent *e);
void checkKeys(XEvent *e);
void render(void);

void drawFlipper(const Flipper &);
void physics(void);
void flipperMovement(Flipper &e);
void flipperBallCollision(Flipper &, Ball &);

int done=0;
int xres=780, yres=480;
int leftButtonDown=0;
Vec leftButtonPos;

GameBoard board;
Curve curve, curve2;

Ball ball1;
Ball ball2;
Flipper flipper;
Flipper flipper2;
Rectangle r;
TreasureChest chest;

Ppmimage *OceanImage;

Ppmimage *pinballImage;
Ppmimage *flippers;
Ppmimage *flippers2;
Ppmimage  *closeChestImage;
Ppmimage *openChestImage;
char ImageFile[NUM_IMAGES][250] = {
    "flippers.png\0",
    "flippers2.jpg\0",
    "pinball.png\0",
    "open-chest2.jpg\0",
    "close-chest2.jpg\0",
    "Ocean.jpg\0",
};/*
strncpy(ImageFile[0],"convert ./images/flippers.jpg ./images/flippers.ppm",
strlen("convert ./images/flippers.jpg ./images/flippers.ppm")+1);
strncpy(ImageFile[1], "convert ./images/flippers2.jpg ./images/flippers2.ppm",
strlen("convert ./images/flippers2.jpg ./images/flippers2.ppm")+1);
strncpy(ImageFile[2], "convert ./images/pinball.png ./images/pinball.ppm",
strlen("convert ./images/pinball.jpg ./images/pinball.ppm")+1);
strncpy(ImageFile[3], "convert ./images/open-chest2.jpg ./images/open-chest2.ppm", 
strlen("convert ./images/open-chest2.jpg ./images/open-chest.ppm")+1);
strncpy(ImageFile[4], "convert ./images/close-chest2.jpg ./images/close-chest2.ppm", 
strlen("convert ./images/close-chest2.jpg ./images/close-chest2.ppm")+1);
strncpy(ImageFile[5], "convert ./images/Ocean.jpg ./images/Ocean.ppm", 
strlen("convert ./images/Ocean.jpg ./images/Ocean.ppm")+1);
*/
GLuint OceanTexture;

GLuint flippersTexture;
GLuint flippersTexture2;
GLuint pinballTexture;
GLuint closeChestTexture;
GLuint openChestTexture;
GLuint openChestTexture_alpha;
GLuint closeChestTexture_alpha;

//------------------------OPENAL-----------------//
//variables below are for AL sound
ALuint alBuffer;
ALuint alSource;
/* functions for openAL:
   clean_sound(alBuffer, alSource);//cleans/deletes sound//use at end of program
//play_sound(alSource)//plays sound, use whenever playing sound
//init_sounds(alBuffer, alSource)// initialize
 */
//-----------------------------------------------------------------------------
//Setup timers
const double physicsRate = 1.0 / 60.0;
const double oobillion = 1.0 / 1e9;
struct timespec timeStart, timeCurrent;
struct timespec timePause;
double physicsCountdown=0.0;
double timeSpan=0.0;
double timeDiff(struct timespec *start, struct timespec *end) {
	return (double)(end->tv_sec - start->tv_sec ) +
		(double)(end->tv_nsec - start->tv_nsec) * oobillion;
}
void timeCopy(struct timespec *dest, struct timespec *source) {
	memcpy(dest, source, sizeof(struct timespec));
}
//-----------------------------------------------------------------------------


int main(void)
{
    char syscall_buffer[256];
    char filename[256];

    for(int i = 0; i < NUM_IMAGES; i++) {
        strcpy(filename, ImageFile[i]);
        char *period = strchr(filename, '.');
        *period = '\0';   
        sprintf(syscall_buffer, "convert ./images/%s ./images/%s.ppm", 
            ImageFile[i], filename);
    
        system(syscall_buffer);
    }
	initXWindows();
	initOpengl();
	initGameBoard(board);
	initBalls();
	initChest(chest);//initialize chest properties
	init_sound(alBuffer, alSource);

	r.pos[0] = 200.0;
	r.pos[1] = 220.0;
	r.width = 100.0;
	r.height = 5.0;
	r.angle = 0.0;

	curve.points[0][0] = xres; curve.points[0][1] = 500;
	curve.points[1][0] = xres; curve.points[1][1] = 600;
	curve.points[2][0] = xres - 100; curve.points[2][1] = 600;
	curve.width = 8.0;
	curve.npoints = 10;


	addCurve(curve, board);

	Rectangle *rec = &board.rectangles[board.num_rectangles];
	rec->pos[0] = xres;
	rec->pos[1] = 300;
	rec->height = 200;
	rec->width = 8.0;
	board.num_rectangles++;

	rec = &board.rectangles[board.num_rectangles];
	rec->pos[0] = xres - 50;
	rec->pos[1] = 300;
	rec->height = 200;
	rec->width = 8.0;
	board.num_rectangles++;



	std::cout << "num rectangles " << board.num_rectangles << endl;

	initFlipper(flipper, 170, 100, false);
	initFlipper(flipper2, xres - 170, 100, true);

	clock_gettime(CLOCK_REALTIME, &timePause);
	clock_gettime(CLOCK_REALTIME, &timeStart);
	while(!done) {
		while(XPending(dpy)) {
			XEvent e;
			XNextEvent(dpy, &e);
			checkResize(&e);
			checkMouse(&e);
			checkKeys(&e);
		}
		clock_gettime(CLOCK_REALTIME, &timeCurrent);
		timeSpan = timeDiff(&timeStart, &timeCurrent);
		timeCopy(&timeStart, &timeCurrent);
		physicsCountdown += timeSpan;
		while(physicsCountdown >= physicsRate) {
			physics();
			physicsCountdown -= physicsRate;
		}
		render();
		glXSwapBuffers(dpy, win);
	}
	cleanupXWindows();
	cleanup_fonts();
	clean_sound(alBuffer, alSource);
    for(int i = 0; i < 6; i++) {
        strcpy(filename, ImageFile[i]);
        char *period = strchr(filename, '.');
        *period = '\0';   
        sprintf(syscall_buffer, "images/%s.ppm", filename);
        unlink(syscall_buffer);
    }
	return 0;
}

void cleanupXWindows(void)
{
	XDestroyWindow(dpy, win);
	XCloseDisplay(dpy);
}

void setTitle(void)
{
	//Set the window title bar.
	XMapWindow(dpy, win);
	XStoreName(dpy, win, "CS335 Demo Program");
}

void setupScreenRes(const int w, const int h)
{
	xres = w;
	yres = h;
}

void initXWindows(void)
{
	GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
	//GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, None };
	Colormap cmap;
	XSetWindowAttributes swa;

	setupScreenRes(480, 700);
	dpy = XOpenDisplay(NULL);
	if (dpy == NULL) {
		printf("\n\tcannot connect to X server\n\n");
		exit(EXIT_FAILURE);
	}
	Window root = DefaultRootWindow(dpy);
	XVisualInfo *vi = glXChooseVisual(dpy, 0, att);
	if (vi == NULL) {
		printf("\n\tno appropriate visual found\n\n");
		exit(EXIT_FAILURE);
	} 
	//else {
	//	// %p creates hexadecimal output like in glxinfo
	//	printf("\n\tvisual %p selected\n", (void *)vi->visualid);
	//}
	cmap = XCreateColormap(dpy, root, vi->visual, AllocNone);
	swa.colormap = cmap;
	swa.event_mask = ExposureMask | KeyPressMask |
		KeyReleaseMask | PointerMotionMask |
		ButtonPressMask | ButtonReleaseMask |
		StructureNotifyMask | SubstructureNotifyMask;
	win = XCreateWindow(dpy, root, 0, 0, xres, yres, 0,
			vi->depth, InputOutput, vi->visual,
			CWColormap | CWEventMask, &swa);
	GLXContext glc = glXCreateContext(dpy, vi, NULL, GL_TRUE);
	glXMakeCurrent(dpy, win, glc);
	setTitle();
	XAutoRepeatOn(dpy);
}

void reshapeWindow(int width, int height)
{
	//window has been resized.
	setupScreenRes(width, height);
	//
	glViewport(0, 0, (GLint)width, (GLint)height);
	glMatrixMode(GL_PROJECTION); glLoadIdentity();
	glMatrixMode(GL_MODELVIEW); glLoadIdentity();
	glOrtho(0, xres, 0, yres, -1, 1);
	setTitle();
}

void initOpengl(void)
{
	//OpenGL initialization
	glViewport(0, 0, xres, yres);
	//Initialize matrices
	glMatrixMode(GL_PROJECTION); glLoadIdentity();
	glMatrixMode(GL_MODELVIEW); glLoadIdentity();
	//This sets 2D mode (no perspective)
	glOrtho(0, xres, 0, yres, -1, 1);
	//Clear the screen
	glClearColor(1.0, 1.0, 1.0, 1.0);
	//Do this to allow fonts
	glEnable(GL_TEXTURE_2D);
	initialize_fonts();


	flipperstexture();
	pinballTextureInit();
    chestTextureInit();	
	OceanTextureInit();	
}

void initBalls(void)
{
	ball1.pos[0] = 100;
	ball1.pos[1] = 200;
	//ball1.vel[0] = 1.6;
	//ball1.vel[1] = 0.0;
	ball1.radius = 11.0;
	ball1.mass = 1.0;

}

void initFlipper(Flipper &f, float xpos, float ypos, bool inverted=false)
{
	f.pos[0] = xpos;
	f.pos[1] = ypos;

	f.angle = FLIPPER_REST_ANGLE;
	f.flipstate = 0;
	f.rvel = 0;
	f.inverted = inverted;
}

void checkResize(XEvent *e)
{
	//The ConfigureNotify is sent by the
	//server if the window is resized.
	if (e->type != ConfigureNotify)
		return;
	XConfigureEvent xce = e->xconfigure;
	if (xce.width != xres || xce.height != yres) {
		//Window size did change.
		reshapeWindow(xce.width, xce.height);
	}
}

void checkMouse(XEvent *e)
{
	//Did the mouse move?
	//Was a mouse button clicked?
	static int savex = 0;
	static int savey = 0;
	//
	if (e->type == ButtonRelease) {
		leftButtonDown=0;
		return;
	}
	if (e->type == ButtonPress) {
		if (e->xbutton.button==1) {
			//Left button is down
			leftButtonDown = 1;
			leftButtonPos[0] = (Flt)e->xbutton.x;
			leftButtonPos[1] = (Flt)(yres - e->xbutton.y);
		}
		if (e->xbutton.button==3) {
			//Right button is down
		}
	}
	if (savex != e->xbutton.x || savey != e->xbutton.y) {
		//Mouse moved
		savex = e->xbutton.x;
		savey = e->xbutton.y;
		if (leftButtonDown) {
			leftButtonPos[0] = (Flt)e->xbutton.x;
			leftButtonPos[1] = (Flt)(yres - e->xbutton.y);
		}
	}
}

void checkKeys(XEvent *e)
{
	int key = XLookupKeysym(&e->xkey, 0);

	//XEvent nev;
	//XPeekEvent(dpy, &nev);
	//Was there input from the keyboard?
	if (e->type == KeyPress) {
		switch(key) {
			case XK_Left:
				ball1.vel[0] -= 1.0;
				break;
			case XK_Right:
				ball1.vel[0] += 1.0;
				break;
			case XK_Up:
				ball1.vel[1] = 20.0;
				break;
			case XK_Down:
				ball1.vel[1] -= 1.0;
				break;
			case XK_s:
				//press s to slow the balls
				ball1.vel[0] *= 0.5;
				ball1.vel[1] *= 0.5;
				ball2.vel[0] *= 0.5;
				ball2.vel[1] *= 0.5;
				break;
			case XK_f:
				//press s to slow the balls
				flipper.flipstate = 1;
				play_sound(alSource);
				break;
			case XK_k:
				//flipper 2
				flipper2.flipstate = 1;
				play_sound(alSource);
				break;
			case XK_Escape:
				done=1;
				break;
		}

	}
	else if (e->type == KeyRelease)
	{
		char keys[32];
		XQueryKeymap(dpy, keys);

		if(!(keys[e->xkey.keycode>>3] & (0x1 << (e->xkey.keycode % 8))))
		{
			// Stuff to do on KeyRelease
			switch (key) {
				case XK_f:
					//press s to slow the balls
					flipper.flipstate = 2;
					break;
				case XK_k:
					//flipper 2
					flipper2.flipstate = 2;
					break;
			}
		}
		//if (nev.type != KeyPress || nev.xkey.time != e->xkey.time ||
		//    nev.xkey.keycode != e->xkey.keycode) {
		//}
	}
}



//moves the flipper based on its flipstate
void flipperMovement(Flipper &f)
{
	//add rotational velocity to angle
	f.angle += f.rvel;

	switch (f.flipstate)
	{
		//idle; rotational velocity is 0 and the angle is the rest angle
		case 0:
			f.rvel = 0;
			f.angle = FLIPPER_REST_ANGLE;
			break;
		case 1:
			//going up; set rotational velocity to flipper speed until the flipper passes the max angle
			if (f.angle < 40) {
				f.rvel = FLIPPER_SPEED;
			}
			else {
				// if the flipper is passed the max angle, set flipstate to 'going down'
				f.rvel = 0;
				//f.flipstate = 2;
			}
			break;
		case 2: 
			//going down; make rotational velocity negative until flipper is at rest angle
			if (f.angle > FLIPPER_REST_ANGLE) {
				f.rvel = -10;
			}
			else {
				f.rvel = 0;
				f.flipstate = 0;
			}
			break;
	}
}

void physics(void)
{

	//gravity
	ball1.vel[1] += -0.2;

	//flipper physics
	flipperMovement(flipper);
	flipperBallCollision(flipper, ball1);
	//flipper2 physics
	flipperMovement(flipper2);
	flipperBallCollision(flipper2, ball1);

	//rectangle collisions
	bool collided = false;
	for (int i = 0; i < board.num_rectangles; i++) {
		if (rectangleBallCollision(board.rectangles[i], ball1)) {
			collided = true;

		}
	}


	if (rectangleBallCollision(chest.r, ball1)) {

		if(ballChestCollision(chest, ball1, alSource)) {
			collided = true;
		}
	}	

	if (collided) {
		//apply roll
		double momentum = ball1.vel[0] * 0.6;
		Vec mv;
		MakeVector(1, 0, 0, mv);
		VecScale(mv, momentum, mv);
		VecAdd(ball1.vel, mv, ball1.vel);

	}

	applyMaximumVelocity(ball1);

	
    //Update position
	ball1.pos[0] += ball1.vel[0];
	ball1.pos[1] += ball1.vel[1];
	if (leftButtonDown) {
		//make ball go toward mouse pointer position
		ball1.vel[0] = (leftButtonPos[0] - ball1.pos[0]) * 0.5;
		ball1.vel[1] = (leftButtonPos[1] - ball1.pos[1]) * 0.5;
	}
	//Check for collision with window edges
	if (ball1.pos[0] < ball1.radius && ball1.vel[0] < 0.0) {
		ball1.pos[0] = ball1.radius;
		ball1.vel[0] *= -0.2;
	}
	else if (ball1.pos[0] >= (Flt)xres-ball1.radius && ball1.vel[0] > 0.0) {
		ball1.pos[0] = xres - ball1.radius;
		ball1.vel[0] *= - 0.2;
	}
	if (ball1.pos[1] < ball1.radius && ball1.vel[1] < 0.0) {
		ball1.pos[1] = ball1.radius;
		ball1.vel[1] *= - 0.2;
	}
	else if	(ball1.pos[1] >= (Flt)yres-ball1.radius && ball1.vel[1] > 0.0) {
		ball1.pos[1] = yres - ball1.radius;
		ball1.vel[1] *= -0.2;
	}
}

void flipperBallCollision(Flipper &f, Ball &b)
{
	float angle = f.inverted ? -f.angle : f.angle;
	//unit axis vectors
	Vec vert, horz;
	MakeVector(0, 1, 0, vert);
	MakeVector(1, 0, 0, horz);
	if (f.inverted)
	{
		VecScale(horz, -1, horz);
	}

	//rotated
	VecRotate(vert, angle, vert);
	VecRotate(horz, angle, horz);

	Vec between;
	VecBtn(f.pos, b.pos, between);
	double projectX = VecProject(between, horz);
	double projectY = VecProject(between, vert);

	//check collision
	if (projectX > 0 && projectX < FLIPPER_LENGTH + b.radius
			&& projectY > -(b.radius + FLIPPER_HEIGHT) && projectY < b.radius) {


		//adjust position
		Vec dP;
		VecScale(vert, b.radius - projectY, dP);
		VecAdd(b.pos, dP,b.pos);


		Vec dV;
		double speed;

		//speed is based on the incoming velocity
		//the horizontal distance from the pivot point of the flipper (torque)
		//and the speed at which the flipper is rotating 
		speed = 0.5 * VecMagnitude(b.vel) + pow((projectX / FLIPPER_LENGTH) + 0.5, 4) * (f.rvel / 10);
		VecScale(vert, speed, dV);
		VecAdd(b.vel, dV, b.vel);

		//reflection vector
		VecRotate(dV, VecAngleBtn(between, vert), dV); 



		//add horizontal velocity if at the tip of flipper
		if (projectX / FLIPPER_LENGTH > 0.20)
		{
			VecScale(horz, speed * 0.2 * projectX / FLIPPER_LENGTH, dV);
			VecAdd(b.vel, dV, b.vel);
		}


	}


}


void render(void)
{


	Rect re;
	glClear(GL_COLOR_BUFFER_BIT);

	//Curve
	glLineWidth(2.5);
	glColor3ub(150, 10, 10);
	glPushMatrix();
	glBegin(GL_LINES);
	glVertex3f(curve.points[0][0], curve.points[0][1],0);
	glVertex3f(curve.points[1][0], curve.points[1][1],0);
	glEnd();
	glBegin(GL_LINES);
	glVertex3f(curve.points[1][0], curve.points[1][1], 0);
	glVertex3f(curve.points[2][0], curve.points[2][1], 0);
	glEnd();


	glColor3ub(150, 10, 10);
	glPushMatrix();
	glTranslated(r.pos[0], r.pos[1], r.pos[2]);
	glRotatef(r.angle, 0, 0, 1);

	for (int i = 0; i < board.num_rectangles; i++)
	{
		drawRectangle(board.rectangles[i]);
	}
    glBegin(GL_QUADS);
    glVertex2i(-r.width, -r.height);
    glVertex2i(-r.width, r.height);
    glVertex2i(r.width, r.height);
    glVertex2i(r.width, - r.height);
    glEnd();
    glPopMatrix();

    OceanBackground();

    for (int i = 0; i < board.num_rectangles; i++)
    {
        drawRectangle(board.rectangles[i]);
    }

	drawChest(chest);//drawing chest

	//draw balls
	glColor3f(1,1,1);
	glPushMatrix();
	glTranslated(ball1.pos[0], ball1.pos[1], ball1.pos[2]);
	drawBall();
	glPopMatrix();

	drawFlipper(flipper);
	drawFlipper(flipper2);

	//glPopMatrix();



	//
	re.bot = yres - 20;
	re.left = 10;
	re.center = 0;
	ggprint8b(&re, 16, 0x0000000, "cs335 - Collision Demo");
	ggprint8b(&re, 16, 0x0000000, "Arrows/mouse to move");
	ggprint8b(&re, 16, 0x0000000, "S - Slow down movement");
	//
}


