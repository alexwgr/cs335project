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

typedef double Flt;
typedef Flt Vec[3];
#define MakeVector(x,y,z,v) (v)[0]=(x),(v)[1]=(y),(v)[2]=(z)
#define VecNegate(a) (a)[0]=(-(a)[0]); (a)[1]=(-(a)[1]); (a)[2]=(-(a)[2]);
#define VecDot(a,b) ((a)[0]*(b)[0]+(a)[1]*(b)[1]+(a)[2]*(b)[2])
#define VecCopy(a,b) (b)[0]=(a)[0];(b)[1]=(a)[1];(b)[2]=(a)[2];

double RadiansToDegrees(double a) {return a * 180.0 / M_PI;}
double DegreesToRadians(double a) {return a * M_PI / 180.0;}

//Vector Functions
double VecMagnitude(Vec vec)
{
	return sqrt(VecDot(vec, vec));

}

double VecProject(Vec vec1, Vec vec2)
{
	return VecDot(vec1, vec2) / VecMagnitude(vec2);
}

double VecAngleBtn(Vec vec1, Vec vec2)
{
	return RadiansToDegrees(acos(VecDot(vec1, vec2) / (VecMagnitude(vec1) * VecMagnitude(vec2))));
}

void VecScale(Vec vec, double scale, Vec out)
{
	out[0] = vec[0] * scale;
	out[1] = vec[1] * scale;
}

void VecRotate(Vec vec, double angle, Vec out)
{
	angle = DegreesToRadians(angle);
	float x = vec[0], y = vec[1];
	out[0] = x * cos(angle) - y * sin(angle);
	out[1] = x * sin(angle) + y * cos(angle);
}
void VecAdd(Vec a, Vec b, Vec out)
{
	out[0] = a[0] + b[0];
	out[1] = a[1] + b[1];
	out[2] = a[2] + b[2];
}

void VecBtn(Vec a, Vec b, Vec out)
{
	out[0] = b[0] - a[0];
	out[1] = b[1] - a[1];
	out[2] = b[2] - a[2];
}


typedef struct t_flipper {
	Vec pos;
	Flt angle;
	Flt rvel;
    //direction inverted
    bool inverted;
	//rotational velocity; gets added to angle every frame
	int flipstate;
	//0 - idle, 1 - going up, 2 - going down
} Flipper;

typedef struct t_Ball {
	Vec pos;
	Vec vel;
	double radius;
	double mass;
} Ball;
#define FLIPPER_LENGTH 100.0
#define FLIPPER_SPEED 10.8
#define FLIPPER_REST_ANGLE -50

//X Windows variables
Display *dpy;
Window win;

void initXWindows(void);
void initOpengl(void);
void initBalls(void);
void initFlipper(Flipper &f);
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
int xres=640, yres=480;
int leftButtonDown=0;
Vec leftButtonPos;


Ball ball1;
Ball ball2;
Flipper flipper;
Flipper flipper2;

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
	initXWindows();
	initOpengl();
	initBalls();

	initFlipper(flipper);
	flipper.inverted = false;
    initFlipper(flipper2);
	flipper2.pos[0] = 200;
	flipper2.pos[1] = 100;
    flipper2.inverted = true;

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

	setupScreenRes(640, 480);
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
}

void initBalls(void)
{
	ball1.pos[0] = 100;
	ball1.pos[1] = 200;
	//ball1.vel[0] = 1.6;
	//ball1.vel[1] = 0.0;
	ball1.radius = 10.0;
	ball1.mass = 1.0;

	ball2.pos[0] = 400;
	ball2.pos[1] = 230;
	ball2.vel[0] = -1.6;
	ball2.vel[1] = 0.0;
	ball2.radius = 10.0;
	ball2.mass = 1.0;
}

void initFlipper(Flipper &f)
{
	f.pos[0] = 0;
	f.pos[1] = 200;

	f.angle = FLIPPER_REST_ANGLE;
	f.flipstate = 0;
	f.rvel = 0;
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
				ball1.vel[1] += 1.0;
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
				break;
			case XK_k:
				//flipper 2
				flipper2.flipstate = 1;
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

void VecNormalize(Vec v)
{
	Flt dist = v[0]*v[0]+v[1]*v[1]+v[2]*v[2];
	if (dist==0.0)
		return;
	Flt len = 1.0 / sqrt(dist);
	v[0] *= len;
	v[1] *= len;
	v[2] *= len;
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
	//flipper physics
	flipperMovement(flipper);
	flipperBallCollision(flipper, ball1);
	//flipper2 physics
	flipperMovement(flipper2);
	flipperBallCollision(flipper2, ball1);
	/*
	//check for collision here
	Flt distance, speed;
	Flt tx, ty;
	Flt movi[2], movj[2];
	Flt massFactor, massi, massj;
	Vec vcontact[2];
	Vec vmove[2];
	Flt dot0,dot1;
	tx = ball1.pos[0] - ball2.pos[0];
	ty = ball1.pos[1] - ball2.pos[1];
	distance = sqrt(tx * tx + ty * ty);
	if (distance < (ball1.radius+ball2.radius)) {
	//We have a collision!
	//vector from center to center.
	vcontact[0][0] = tx;
	vcontact[0][1] = ty;
	vcontact[0][2] = 0.0;
	VecNormalize(vcontact[0]);
	VecCopy(vcontact[0],vcontact[1]);
	VecNegate(vcontact[1]);
	movi[0] = ball1.vel[0];
	movi[1] = ball1.vel[1];
	movj[0] = ball2.vel[0];
	movj[1] = ball2.vel[1];
	vmove[0][0] = movi[0];
	vmove[0][1] = movi[1];
	vmove[0][2] = 0.0;
	vmove[1][0] = movj[0];
	vmove[1][1] = movj[1];
	vmove[1][2] = 0.0;
	VecNormalize(vmove[0]);
	VecNormalize(vmove[1]);
	//Determine how direct the hit was...
	dot0 = VecDot(vcontact[0], vmove[0]);  //dot product
	dot1 = VecDot(vcontact[1], vmove[1]);  //dot product
	//Find the closing (relative) speed of the objects...
	Flt speed0 = sqrtf( movi[0]*movi[0] + movi[1]*movi[1]) * dot0;
	Flt speed1 = sqrtf( movj[0]*movj[0] + movj[1]*movj[1]) * dot1;
	speed = speed0 + speed1;
	if (speed < 0.0) {
	//Normalize the mass of each object...
	massFactor = 2.0 / (ball1.mass + ball2.mass);
	massi = ball1.mass * massFactor;
	massj = ball2.mass * massFactor;
	ball2.vel[0] += vcontact[0][0] * speed * massi;
	ball2.vel[1] += vcontact[0][1] * speed * massi;
	ball1.vel[0] += vcontact[1][0] * speed * massj;
	ball1.vel[1] += vcontact[1][1] * speed * massj;
	}
	}
	 */

	//gravity
	ball1.vel[1] += -0.1;

	//Update position
	ball1.pos[0] += ball1.vel[0];
	ball1.pos[1] += ball1.vel[1];
	if (leftButtonDown) {
		//make ball go toward mouse pointer position
		ball1.vel[0] = (leftButtonPos[0] - ball1.pos[0]) * 0.5;
		ball1.vel[1] = (leftButtonPos[1] - ball1.pos[1]) * 0.5;
	}
	//Check for collision with window edges
	if ((ball1.pos[0] < ball1.radius && ball1.vel[0] < 0.0) ||
			(ball1.pos[0] >= (Flt)xres-ball1.radius && ball1.vel[0] > 0.0)) {
		ball1.vel[0] = -ball1.vel[0];
	}
	if ((ball1.pos[1] < ball1.radius && ball1.vel[1] < 0.0) ||
			(ball1.pos[1] >= (Flt)yres-ball1.radius && ball1.vel[1] > 0.0)) {
		ball1.vel[1] = -ball1.vel[1];
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
			&& projectY > -(b.radius + 20) && projectY < b.radius) {
		Vec dP;
		VecScale(vert, b.radius - projectY, dP);
		VecAdd(b.pos, dP, b.pos);

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
		/* if (projectX > FLIPPER_LENGTH - 5.0)
		   {
		   VecScale(horz, speed * 0.4, dV);
		   VecAdd(b.vel, dV, b.vel);
		   }*/


	}


}

void drawBall(Flt rad)
{
	int i;
	static int firsttime=1;
	static Flt verts[32][2];
	static int n=32;
	if (firsttime) {
		Flt ang=0.0;
		Flt inc = 3.14159 * 2.0 / (Flt)n;
		for (i=0; i<n; i++) {
			verts[i][0] = sin(ang);
			verts[i][1] = cos(ang);
			ang += inc;
		}
		firsttime=0;
	}
	glBegin(GL_TRIANGLE_FAN);
	for (i=0; i<n; i++) {
		glVertex2f(verts[i][0]*rad, verts[i][1]*rad);
	}
	glEnd();
}


void drawFlipper(const Flipper &f)
{
    float length = f.inverted ? -FLIPPER_LENGTH : FLIPPER_LENGTH;
    float angle = f.inverted ? -f.angle : f.angle;
    glPushMatrix();
	glColor3ub(1, 140, 0);
	glTranslatef(f.pos[0], f.pos[1], f.pos[2]);
	glRotatef(angle, 0, 0, 1);
	glBegin(GL_QUADS);
	glVertex2f(0, -20.0);
	glVertex2f(length, -20.0);
	glVertex2f(length, 0);
	glVertex2f(0, 0);
	glEnd();
    glPopMatrix();
}

void render(void)
{
	Rect r;
	glClear(GL_COLOR_BUFFER_BIT);

	//draw balls
	glColor3ub(30,60,90);
	glPushMatrix();
	glTranslatef(ball1.pos[0], ball1.pos[1], ball1.pos[2]);
	drawBall(ball1.radius);
	glPopMatrix();


    drawFlipper(flipper);
    drawFlipper(flipper2);

	//glPopMatrix();



	//
	r.bot = yres - 20;
	r.left = 10;
	r.center = 0;
	ggprint8b(&r, 16, 0x0000000, "cs335 - Collision Demo");
	ggprint8b(&r, 16, 0x0000000, "Arrows/mouse to move");
	ggprint8b(&r, 16, 0x0000000, "S - Slow down movement");
	//
}



