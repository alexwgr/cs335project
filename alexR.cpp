// Author: Alex Rinaldi 
// CS335 Project 

/* Roles for this project:
 * Collision detection
 * Physics and movement 
 * Special effects 
 */


#include "vector.h"
#include "gameObjects.h"
#include "alexR.h"
#include <iostream>
#include <cmath>
#include <GL/glx.h>

#define MAX_VELOCITY 30.0

void initGameBoard(GameBoard &g)
{
    g.num_rectangles = 0;
}

/* This draws rectangles for debugging */
void drawRectangle(Rectangle &r)
{

    glColor3ub(150, 10, 10);
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
/*void drawCircle(Circle &c)
{
    glColor3ub(150, 10, 10);
    glPushMatrix();
}*/

/* This adds a curve to the game board */
/* It uses beizer curves to rectangles at a set number of steps */
void addCurve(Curve &c, GameBoard &g)
{
    //uses beizer curves
    double t = 0.0;
    double tStep = 1.0 / c.npoints;
    double x0, y0, x1, y1;
    Vec point1, point2, btn, horz, zero;
    MakeVector(1, 0, 0, horz);
    MakeVector(0, 0, 0, zero);
    
    
    for (int i = 0; i <= c.npoints; i++) {

        x0 = (1.0 - t) * (1.0 - t) * c.points[0][0] +
            2.0 * (1.0 - t) * t * c.points[1][0] + 
            t * t * c.points[2][0];

        y0 = (1.0 - t) * (1.0 - t) * c.points[0][1] +
            2.0 * (1.0 - t) * t * c.points[1][1] + 
            t * t * c.points[2][1];

        if (i > 0) {
        //for each segment of the curve, draw a rectangle
            Rectangle *r = &(g.rectangles[g.num_rectangles]);
            MakeVector(x0, y0, 0, point1);
            MakeVector(x1, y1, 0, point2);
            VecBtn(point1, point2, btn);
            r->pos[0] = (x0 + x1) / 2.0;
            r->pos[1] = (y0 + y1) / 2.0;
            
            r->collide[0] = c.collide[0];
            r->collide[1] = c.collide[1];
            r->collide[2] = false;
            r->collide[3] = false;

            //rotate the rectangle based on the angle its horizontal edge and the x-axis
            r->angle = (isLeft(zero, horz, btn) ? -1 : 1) * VecAngleBtn(horz, btn);
            r->width = VecMagnitude(btn) / 2.0;
            r->width += r->width * .05;
            r->height = c.width;
            g.num_rectangles++;
        }
        x1 = x0;
        y1 = y0;

        t += tStep;

    }
}

/* Vector function for testing if a point is on either side of a line */
bool isLeft(Vec &a, Vec &b, Vec &p)
{
    Vec v1, v2;
    VecBtn(a, b, v1);
    VecBtn(p, b, v2);
    double xp = v1[0] * v2[1] - v1[1] * v2[0];
    return xp > 0;
}


/* Given a position and an angle, get coordinates of rectangle corners */
void getRectangleCorners(Rectangle &r,
        Vec &corner1, Vec &corner2, Vec &corner3, Vec &corner4)
{
    Vec vert, horz, neg_vert, neg_horz;
    MakeVector(0,1,0,vert);
    MakeVector(1,0,0, horz);


    //unit vectors rotated
    VecRotate(vert, r.angle, vert);
    VecRotate(horz, r.angle, horz);

    //scaled to rectangle width and height
    VecScale(vert, r.height, vert);
    VecScale(horz, r.width, horz);
    VecScale(vert, -1, neg_vert);
    VecScale(horz, -1, neg_horz);

    //get rectangle relative corners
    VecAdd(neg_horz, vert, corner1);
    VecAdd(horz, vert, corner2);
    VecAdd(horz, neg_vert, corner3);
    VecAdd(neg_horz, neg_vert, corner4);


    //shift by rectangle position
    VecAdd(corner1, r.pos, corner1);
    VecAdd(corner2, r.pos, corner2);
    VecAdd(corner3, r.pos, corner3);
    VecAdd(corner4, r.pos, corner4);


}


/* Handles the physics for a circle colliding with a rectangle */
int rectangleBallCollision(Rectangle &r, Ball &b)
{
    Vec zero;
    MakeVector(0, 0, 0, zero);

    Vec corner[4];

    float angle = r.angle;
    //unit axis vectors
    Vec vert, horz, gravity;
    MakeVector(0, 1, 0, vert);
    MakeVector(1, 0, 0, horz);

    MakeVector(0, -1, 0, gravity);


    //rotated
    VecRotate(vert, angle, vert);
    VecRotate(horz, angle, horz);




    Vec between, dV, rNorm;
    VecBtn(r.pos, b.pos, between);
    double projectX = VecProject(between, horz);
    double projectY = VecProject(between, vert);
    double currentSpeed = VecMagnitude(b.vel);

    //check collision
    if (projectX > -(r.width + b.radius) && projectX < r.width + b.radius
            && projectY > -(b.radius + r.height) && projectY < b.radius + r.height) {

        getRectangleCorners(r, corner[0], corner[1], corner[2], corner[3]);


        //Figure out which edge
        bool lDiagonal = isLeft(corner[0], corner[2], b.pos);
        bool rDiagonal = isLeft(corner[3], corner[1], b.pos);

        if (lDiagonal && rDiagonal) {
            if (!r.collide[1])
                return 0;

            //bottom edge
            VecScale(vert, -1, rNorm);
        }
        else if (lDiagonal && !rDiagonal) {
            if (!r.collide[2])
                return 0;

            //left edge
            VecScale(horz, -1, rNorm);
        }
        else if (!lDiagonal && rDiagonal) {
            if (!r.collide[3])
                return 0;

            //right edge
            VecScale(horz, 1, rNorm);
        }
        else {
            if (!r.collide[0])
                return 0;

            //top edge
            VecScale(vert, 1, rNorm);

        }


        //if (VecProject(b.vel, rNorm) < 0 && currentSpeed < 0.1)
        //{
        //VecScale(rNorm, std::abs(currentSpeed) * projectY * 0.5, b.vel);
        //}

        //else
        //{

        Vec dP;
        double projectNorm = VecProject(between, rNorm); 
        VecScale(rNorm, std::abs(b.radius - std::abs(projectNorm - r.height)), dP);
        if (VecMagnitude(dP) <= b.radius) {
            VecAdd(b.pos, dP, b.pos);
        }

        VecNormalize(b.vel, dV);
        VecScale(dV, -1, dV);
        double angBtn = VecAngleBtn(rNorm, dV);

        bool posAng = isLeft(zero, rNorm, dV);

        VecRotate(dV,(posAng ? 2 : -2) * angBtn, dV);
        VecScale(dV, 0.6 * currentSpeed, b.vel);

        return 1;
    }

    return 0;
}

/* Scales down the ball's velocity if it is greater than maximum */
void applyMaximumVelocity(Ball &b)
{

    if (VecMagnitude(b.vel) > MAX_VELOCITY) {
        VecNormalize(b.vel, b.vel);
        VecScale(b.vel, MAX_VELOCITY, b.vel);
    }
}
