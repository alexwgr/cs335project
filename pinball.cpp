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
#include <time.h>
#include <math.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>



extern "C" {
#include "fonts.h"
}

#include "ppm.h"
#include "vector.h"
#include "gameObjects.h"
#include "alexR.h"
#include "omarO.h"
#include "hassenS.h"

const int NUM_IMAGES = 34;
const int SMOKE_SPRITES = 12;
const double CHUTE_THICKNESS = 6.0;
const double CHUTE_WIDTH = 40.0;
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
void initGameBoard(GameBoard &);
void initFlipper(Flipper &, float, float, bool);
void initTextures(void);
void initDeflectors(GameBoard &board);
void cleanupXWindows(void);
void checkResize(XEvent *e);
void checkMouse(XEvent *e);
void checkKeys(XEvent *e);
void render(void);

void drawBumper(Bumper &);
void drawCanon(Canon &);
void drawSmoke(Smoke &);
void drawChest(TreasureChest &);
void drawFlipper(const Flipper &);
void drawSteeringWheel(SteeringWheel &);
void drawDeflector(Deflector &);
void OceanBackground();
void drawFlipper(Flipper &f);
void drawBall();

void physics(void);
void flipperMovement(Flipper &e);

bool hide = false;

int done=0;
int xres=780, yres=480;
int leftButtonDown=0;
bool boom = false;

char buffer[256];

Vec leftButtonPos;

score Scorekeeper;

GameBoard board;
Curve curve, curve2;

//gameObjects
Ball ball1;
Ball ball2;
Flipper flipper, flipper2;
Rectangle r;
SteeringWheel steeringWheel;
TreasureChest chest;
Canon canon;
Smoke smoke;

Ppmimage *OceanImage;

Ppmimage *pinballImage;
Ppmimage *flippers;
Ppmimage *flippers2;
Ppmimage  *closeChestImage;
Ppmimage *openChestImage;
Ppmimage *bumperUpImage;
Ppmimage *bumperDownImage;
Ppmimage *canonImage;
Ppmimage *steeringWheelImage;
Ppmimage *ropeDeflectorImage[2];
Ppmimage *smokeSprites[SMOKE_SPRITES];
char ImageFile[NUM_IMAGES][250] = {
    "flippers.png\0",
    "flippers2.jpg\0",
    "pinball.png\0",
    "open-chest2.png\0",
    "close-chest2.png\0",
    "Ocean.jpg\0",
    "bumper_up.png\0",
    "bumper_down.png\0",
    "canon.png\0",
    "steering_wheel.png\0",
    "flippers.png\0",
    "flippers2.jpg\0",
    "pinball.png\0",
    "open-chest2.png\0",
    "close-chest2.png\0",
    "Ocean.jpg\0",
    "bumper_up.png\0",
    "bumper_down.png\0",
    "canon.png\0",
    "steering_wheel.png\0",
    "rope.png\0",
    "rope_bent.png\0",
    "smoke0.png\0","smoke1.png\0",
    "smoke2.png\0","smoke3.png\0",
    "smoke4.png\0","smoke5.png\0",
    "smoke6.png\0","smoke7.png\0",
    "smoke8.png\0","smoke9.png\0",
    "smoke10.png\0","smoke11.png\0"
};
GLuint OceanTexture;

GLuint flippersTexture;
GLuint flippersTexture2;
GLuint pinballTexture;
GLuint closeChestTexture;
GLuint openChestTexture;
GLuint openChestTexture_alpha;
GLuint closeChestTexture_alpha;
GLuint bumperUpTexture;
GLuint bumperDownTexture;
GLuint canonTexture;
GLuint smokeSpriteTexture[SMOKE_SPRITES];
GLuint steeringWheelTexture;
GLuint ropeDeflectorTexture[2];

//------------------------OPENAL-----------------//
//variables below are for AL sound
ALuint alBuffer;
ALuint alSource;

//Setup timers
const double physicsRate = 1.0 / 60.0;
const double oobillion = 1.0 / 1e9;
struct timespec timeStart, timeCurrent;
struct timespec timePause;
double physicsCountdown=0.0;
double timeSpan=0.0;

int main(void)
{
    Scorekeeper.points = 0;
    Scorekeeper.balls_left = 3;


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
    initDeflectors(board);
    initBumpers(board);
    initBalls();
    initChest(chest);//initialize chest properties
    initCanon(canon);
    initSmoke(smoke);
    initSteeringWheel(steeringWheel);


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


    // Add chute to board
    addCurve(curve, board);

    Rectangle *rec = &board.rectangles[board.num_rectangles];
    rec->pos[0] = xres;
    rec->pos[1] = 300;
    rec->height = 200;
    rec->width = CHUTE_THICKNESS;
    board.num_rectangles++;

    rec = &board.rectangles[board.num_rectangles];
    rec->pos[0] = xres - CHUTE_WIDTH;
    rec->pos[1] = 300;
    rec->height = 200;
    rec->width = 6.0;
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
    for(int i = 0; i < NUM_IMAGES; i++) {
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

    initTextures();
}

void initGameBoard(GameBoard &gb) {
    gb.num_rectangles = 0;

    Rectangle rec;
    rec.angle = -50.0;
    rec.width = 60.0;
    rec.height = 10.0;
    MakeVector(130, 125, 0, rec.pos);
    addRectangleToBoard(rec, gb);
}

void initBalls(void)
{
    ball1.radius = 11.0;
    ball1.pos[0] = xres - CHUTE_THICKNESS - ball1.radius;
    ball1.pos[1] = 200;
    //ball1.vel[0] = 1.6;
    //ball1.vel[1] = 0.0;
    ball1.mass = 1.0;

}

void initDeflectors(GameBoard &board)
{
    Deflector *deflector = &board.deflectors[0];
    MakeVector(140, 200, 0, deflector->r.pos);
    deflector->r.angle = -70.0;
    deflector->r.width = 40;
    deflector->r.height = 5;

    deflector++;
    MakeVector(340, 200, 0, deflector->r.pos);
    deflector->r.angle = 70.0;
    deflector->r.width = 40;
    deflector->r.height = 5;

    board.num_deflectors = 2;
}

void initFlipper(Flipper &f, float xpos, float ypos, bool inverted=false)
{
    f.pos[0] = xpos;
    f.pos[1] = ypos;
    f.width = FLIPPER_LENGTH;
    f.height = FLIPPER_HEIGHT;

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
            case XK_b:
                boom = true;
            case XK_h:
                hide = true;
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

        if (!(keys[e->xkey.keycode>>3] & (0x1 << (e->xkey.keycode % 8))))
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
            //going up; set rotational velocity to 
            //flipper speed until the flipper passes the max angle
            if (f.angle < 40) {
                f.rvel = FLIPPER_SPEED;
            }
            else {
                // if the flipper is passed the max angle, set flipstate to 
                //'going down'
                f.rvel = 0;
                //f.flipstate = 2;
            }
            break;
        case 2: 
            //going down; make rotational velocity negative 
            //until flipper is at rest angle
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

    //treasure chest collision
    if (rectangleBallCollision(chest.r, ball1)) {

        if (chest.active == 1) {
            timeCopy(&chest.collision_time, &timeCurrent);
            chest.active = 0;

            if (ballChestCollision(chest, ball1, alSource)) {
                collided = true;
            }
        }

        if (chest.active == 0 && 
                timeDiff(&chest.collision_time, &timeCurrent) > 0.5) {
            chest.active = 1;
        }

    }	

    //bumper collision
    for (int i = 0; i < board.num_bumpers; i++) {
        if (bumperBallCollision(board.bumpers[i], ball1)) {

        }
    }

    //deflector collisions
    for (int i = 0; i < board.num_rectangles; i++) {
        if (deflectorBallCollision(board.deflectors[i], ball1)) {
        }
    }

    //steering wheel collision
    steeringWheelBallCollision(steeringWheel, ball1);
    steeringWheelMovement(steeringWheel);

    if (collided) {
        //apply roll
        double momentum = ball1.vel[0] * 0.6;
        Vec mv;
        MakeVector(1, 0, 0, mv);
        VecScale(mv, momentum, mv);
        VecAdd(ball1.vel, mv, ball1.vel);

    }

    for (int i = 0; i < board.num_bumpers; i++) {
        if (bumperBallCollision(board.bumpers[i], ball1)) {
            play_sound(alSource);
        }
    }
    KaBoom(canon, ball1, alSource);//when ball is on canon

    applyMaximumVelocity(ball1);

    //set ball inPlay flag if ball is left of launch chute
    if (ball1.pos[0] < xres - CHUTE_WIDTH - CHUTE_THICKNESS) {
        ball1.inPlay = 1;
    }

    //Update position
    ball1.pos[0] += ball1.vel[0];
    ball1.pos[1] += ball1.vel[1];
    if (leftButtonDown) {
        //make ball go toward mouse pointer position
        ball1.vel[0] = (leftButtonPos[0] - ball1.pos[0]) * 0.5;
        ball1.vel[1] = (leftButtonPos[1] - ball1.pos[1]) * 0.5;
    }

    //Check for collision with window edges

    //right board edge shifts left if the ball is in play 
    //and has left the chute
    double boardEdge = ball1.inPlay 
        ? xres - (CHUTE_WIDTH + CHUTE_THICKNESS + ball1.radius + 0.5) 
        : xres - ball1.radius;

    //left window edge
    if (ball1.pos[0] < ball1.radius && ball1.vel[0] < 0.0) {
        ball1.pos[0] = ball1.radius;
        ball1.vel[0] *= -0.2;
    }

    //right window edge
    else if (ball1.pos[0] >= boardEdge && ball1.vel[0] > 0.0) {
        ball1.pos[0] = boardEdge;
        ball1.vel[0] *= - 0.2;
    }

    //bottom window edge
    //Hassen 
    if (ball1.pos[1] < ball1.radius && ball1.vel[1] < 0.0) {
        ball1.pos[1] = ball1.radius;
        ball1.vel[1] *= - 0.2;
    }

    //top window edge
    else if	(ball1.pos[1] >= (Flt)yres-ball1.radius && ball1.vel[1] > 0.0) {
        ball1.pos[1] = yres - ball1.radius;
        ball1.vel[1] *= -0.2;
    }
}

void initTextures(void)
{

    strcpy(buffer, "./images/rope.ppm");
    alphaTextureInit(buffer, ropeDeflectorTexture[0], ropeDeflectorImage[0]);

    strcpy(buffer, "./images/rope_bent.ppm");
    alphaTextureInit(buffer, ropeDeflectorTexture[1], ropeDeflectorImage[1]);

    strcpy(buffer, "./images/pinball.ppm");
    textureInit(buffer, pinballTexture, pinballImage);

    strcpy(buffer, "./images/flippers.ppm");
    textureInit(buffer, flippersTexture, flippers);

    strcpy(buffer, "./images/flippers2.ppm");
    textureInit(buffer, flippersTexture2, flippers2);

    strcpy(buffer, "./images/Ocean.ppm");
    textureInit(buffer, OceanTexture, OceanImage);

    strcpy(buffer , "./images/canon.ppm");
    alphaTextureInit(buffer, canonTexture, canonImage);

    strcpy(buffer, "./images/open-chest2.ppm");
    alphaTextureInit(buffer, openChestTexture_alpha, openChestImage);

    strcpy(buffer, "./images/close-chest2.ppm");
    alphaTextureInit(buffer, closeChestTexture_alpha, closeChestImage);

    strcpy(buffer, "./images/bumper_up.ppm");
    alphaTextureInit(buffer, bumperUpTexture, bumperUpImage);

    strcpy(buffer, "./images/bumper_down.ppm");
    alphaTextureInit(buffer, bumperDownTexture, bumperDownImage);

    strcpy(buffer, "./images/steering_wheel.ppm");
    alphaTextureInit(buffer, steeringWheelTexture, steeringWheelImage);

    strcpy(buffer, "./images/smoke0.ppm");
    alphaTextureInit(buffer, smokeSpriteTexture[0], smokeSprites[0]);

    strcpy(buffer, "./images/smoke1.ppm");
    alphaTextureInit(buffer, smokeSpriteTexture[1], smokeSprites[1]);

    strcpy(buffer, "./images/smoke2.ppm");
    alphaTextureInit(buffer, smokeSpriteTexture[2], smokeSprites[2]);

    strcpy(buffer, "./images/smoke3.ppm");
    alphaTextureInit(buffer, smokeSpriteTexture[3], smokeSprites[3]);

    strcpy(buffer, "./images/smoke4.ppm");
    alphaTextureInit(buffer, smokeSpriteTexture[4], smokeSprites[4]);

    strcpy(buffer, "./images/smoke5.ppm");
    alphaTextureInit(buffer, smokeSpriteTexture[5], smokeSprites[5]);

    strcpy(buffer, "./images/smoke6.ppm");
    alphaTextureInit(buffer, smokeSpriteTexture[6], smokeSprites[6]);

    strcpy(buffer, "./images/smoke7.ppm");
    alphaTextureInit(buffer, smokeSpriteTexture[7], smokeSprites[7]);

    strcpy(buffer, "./images/smoke8.ppm");
    alphaTextureInit(buffer, smokeSpriteTexture[8], smokeSprites[8]);

    strcpy(buffer, "./images/smoke9.ppm");
    alphaTextureInit(buffer, smokeSpriteTexture[9], smokeSprites[9]);

    strcpy(buffer, "./images/smoke10.ppm");
    alphaTextureInit(buffer, smokeSpriteTexture[10], smokeSprites[10]);

    strcpy(buffer, "./images/smoke11.ppm");
    alphaTextureInit(buffer, smokeSpriteTexture[11], smokeSprites[11]);
}

void drawSteeringWheel(SteeringWheel &wheel)
{
    glPushMatrix();
    glColor3d(1.0, 1.0, 1.0);
    glTranslated(wheel.pos[0], wheel.pos[1], wheel.pos[2]);
    glRotatef(wheel.angle, 0, 0, 1);

    glBindTexture(GL_TEXTURE_2D, steeringWheelTexture);

    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.0f);
    glColor4ub(255,255,255,255);

    glBegin(GL_QUADS);
    glVertex2d(-wheel.outer_radius, -wheel.outer_radius); 
    glTexCoord2f(0.0f, 1.0f);
    glVertex2d(-wheel.outer_radius,  wheel.outer_radius); 
    glTexCoord2f(0.0f, 0.0f); 
    glVertex2d( wheel.outer_radius,  wheel.outer_radius); 
    glTexCoord2f(1.0f, 0.0f); 
    glVertex2d( wheel.outer_radius, -wheel.outer_radius); 
    glTexCoord2f(1.0f, 1.0f); 
    glEnd();

    glBindTexture(GL_TEXTURE_2D, 0);
    glPopMatrix();
}

void drawDeflector(Deflector &d)
{
    glPushMatrix();
    glColor3d(1.0, 1.0, 1.0);
    glTranslated(d.r.pos[0], d.r.pos[1], d.r.pos[2]);
    glRotatef(d.r.angle, 0, 0, 1);

    //use d.state to determine which image to bind
    glBindTexture(GL_TEXTURE_2D, ropeDeflectorTexture[d.state]);

    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.0f);
    glColor4ub(255,255,255,255);

    //enlarge the image beyond the width of the rectangle
    //get ratio between image height and rectangle height
    //w = ropeDeflectorImage[0]->width * ratio;
    double w = d.r.width;
    double h = d.r.height * 2;

    glBegin(GL_QUADS);
    glTexCoord2f(1.0f, 0.0f); glVertex2i(-w, -h);
    glTexCoord2f(0.0f, 0.0f); glVertex2i(-w,  h);
    glTexCoord2f(0.0f, 1.0f); glVertex2i( w,  h);
    glTexCoord2f(1.0f, 1.0f); glVertex2i( w, -h);
    glEnd();


    glPopMatrix();
}


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
    glBindTexture(GL_TEXTURE_2D, flippersTexture);
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
void drawBumper(Bumper &b)
{
    double angle, radian, x, y, tx, ty, xcos, ysin;
    double radius = b.state == 0 ? b.c.radius : b.c.radius - 2.0;

    glPushMatrix();
    glColor3f(1.0f, 1.0f, 1.0f);
    glTranslated(b.c.pos[0], b.c.pos[1], b.c.pos[2]);
    glBindTexture(GL_TEXTURE_2D, b.state == 0 
            ? bumperUpTexture 
            : bumperDownTexture);
    glBegin(GL_POLYGON);

    for(angle = 0.0; angle < 360.0; angle+= 2.0) {
        radian = angle * (M_PI/100.0f);

        xcos = (float)cos(radian);
        ysin = (float)sin(radian);
        x = xcos * radius;
        y = ysin * radius;
        tx = xcos * 0.5 + 0.5;
        ty = ysin * 0.5 + 0.5;

        glTexCoord2f(tx, ty);
        glVertex2f(x, y);
    }
    glEnd();
    glPopMatrix();
}
void drawSmoke(Smoke &s)
{		
    if(boom) {
        if (s.frame < SMOKE_SPRITES) {
            cout << s.frame << " \n";//prints frame # to console for debugging
            glPushMatrix();
            glColor3d(1.0, 1.0, 1.0);
            glTranslated(s.r.pos[0], s.r.pos[1], s.r.pos[2]);
            glRotatef(s.r.angle, 0, 0, 1);
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
            s.frame++;	
        } else {
            s.frame = 0;
            boom = false;
        }
    }
}
void drawCanon(Canon &c)
{
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
//function draws treasure chest object
void drawChest(TreasureChest &c)
{
    glPushMatrix();
    glColor3d(1.0, 1.0, 1.0);
    glTranslated(c.r.pos[0], c.r.pos[1], c.r.pos[2]);
    glRotatef(c.r.angle, 0, 0, 1);

    if (c.state == 1) {
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
void render(void)
{
    //	Rect re;
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

    glEnd();
    glPopMatrix();

    OceanBackground();

    drawChest(chest);//drawing chest
    drawSteeringWheel(steeringWheel);
    drawCanon(canon);//draw canon

    if(boom) {
        drawSmoke(smoke);
    }

    //draw collision rectangles
    glColor3ub(255, 255, 255);
    for (int i = 0; i < board.num_rectangles; i++) {
        drawRectangle(board.rectangles[i]);
    }

    //draw deflectors
    for (int i = 0; i < board.num_deflectors; i++) { 
        drawDeflector(board.deflectors[i]);
    }

    //draw bumpers
    for (int i = 0; i < board.num_bumpers; i++) {
        drawBumper(board.bumpers[i]);
    }

    //draw balls
    glColor3f(1,1,1);
    glPushMatrix();
    glTranslated(ball1.pos[0], ball1.pos[1], ball1.pos[2]);
    if (ball1.isVisible) {
        drawBall();
    }
    glPopMatrix();

    drawFlipper(flipper);
    drawFlipper(flipper2);
    if(hide) { 
        drawScore();
    } 
}


