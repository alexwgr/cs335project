#include "vector.h"
#include "gameObjects.h"
#include "alexR.h"
#include <iostream>
#include <cmath>


bool isLeft(Vec &a, Vec &b, Vec &p)
{
    Vec v1, v2;
    VecBtn(a, b, v1);
    VecBtn(p, b, v2);
    double xp = v1[0] * v2[1] - v1[1] * v2[0];
    return xp > 0;
}

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



void rectangleBallCollision(Rectangle &r, Ball &b)
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
        
        if (lDiagonal && rDiagonal)
        {
        //bottom edge
            VecScale(vert, -1, rNorm);
        }
        else if (lDiagonal && !rDiagonal)
        {
        //left edge
            VecScale(horz, -1, rNorm);
        }
        else if (!lDiagonal && rDiagonal)
        {
        //right edge
            VecScale(horz, 1, rNorm);
        }
        else
        {
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
        VecScale(rNorm, b.radius - std::abs(projectNorm - r.height), dP);
        VecAdd(b.pos, dP, b.pos);

        VecNormalize(b.vel, dV);
        VecScale(dV, -1, dV);
        double angBtn = VecAngleBtn(rNorm, dV);

        bool posAng = isLeft(zero, rNorm, dV);

        VecRotate(dV,(posAng ? 2 : -2) * angBtn, dV);
        VecScale(dV, currentSpeed * 0.6, b.vel);
        
        //VecAdd(b.pos, dP, b.pos);
        //}
    }
}


