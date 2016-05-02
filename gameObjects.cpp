#include "gameObjects.h"
#include "vector.h"
#include "alexR.h"
#include <iostream>
#include <cstring>
#include <cmath>
#include <GL/glx.h>

void initGameBoard(GameBoard &g)
{
    g.num_rectangles = 0;
}

void initBumpers(GameBoard &g)
{
    Bumper b;
    MakeVector(300, 500, 0, b.c.pos);
    b.c.radius = 20;
    b.state = 0;

    addBumperToBoard(b, g);

    MakeVector(200, 500, 0, b.c.pos);

    addBumperToBoard(b, g);

    MakeVector(250, 400, 0, b.c.pos);

    addBumperToBoard(b, g);

}

void addBumperToBoard(Bumper &b, GameBoard &g)
{
    Bumper *currentBumper = &g.bumpers[g.num_bumpers];
    currentBumper->c.pos[0] = b.c.pos[0];
    currentBumper->c.pos[1] = b.c.pos[1];
    currentBumper->c.radius = b.c.radius;
    currentBumper->state = b.state;
    g.num_bumpers++;
}


/* This draws rectangles for debugging */
void drawRectangle(Rectangle &r)
{

    glPushMatrix();
    glTranslated(r.pos[0], r.pos[1], r.pos[2]);
    glRotatef(r.angle, 0, 0, 1);
    glBegin(GL_QUADS);
    glVertex2i(-r.width, -r.height);
    glVertex2i(-r.width, r.height);
    glVertex2i(r.width, r.height);
    glVertex2i(r.width, - r.height);
    glEnd();
    glPopMatrix();
}

/* This draws circles for debugging */
void drawCircle(Circle &c)
{
    int i;
    double verts[32][2];
    int n=32;
    double ang=0.0;
    double inc = 3.14159 * 2.0 / (double)n;
    for (i=0; i<n; i++) {
        verts[i][0] = sin(ang);
        verts[i][1] = cos(ang);
        ang += inc;
    }
    glColor3ub(250, 0, 0);
    glBegin(GL_TRIANGLE_FAN);
    for (i=0; i<n; i++) {
        glVertex2f(verts[i][0]*c.radius, verts[i][1]*c.radius);
    }
    glEnd();
}

double timeDiff(struct timespec *start, struct timespec *end) {
    return (double)(end->tv_sec - start->tv_sec ) +
        (double)(end->tv_nsec - start->tv_nsec) * (1.0/1e9);
}

void timeCopy(struct timespec *dest, struct timespec *source) {
    std::memcpy(dest, source, sizeof(struct timespec));
}
