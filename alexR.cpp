#include "vector.h"
#include "gameObjects.h"
#include "alexR.h"
#include <iostream>
#include <cmath>


bool isLeft(Vec &a, Vec &b, Vec &p)
{
    return ((b[0] - a[0]) * (p[1] - a[1]) - (b[1] - a[1])*(p[0] - a[0])) > 0;
}

void rectangleBallCollision(Rectangle &r, Ball &b)
{
    Vec corner[4];
    corner[0][0] = -r.length; corner[0][1] = r.height;
    corner[1][0] = r.length; corner[1][1] = r.height;
    corner[2][0] = r.length; corner[2][1] = -r.height;
    corner[3][0] = -r.length; corner[3][1] = -r.height;

    bool lDiagonal = isLeft(corner[0], corner[2], b.pos);
    bool rDiagonal = isLeft(corner[3], corner[1], b.pos);

    if (lDiagonal && rDiagonal)
    {
        std::cout << "Above" << std::endl;
    }
    else if (lDiagonal && !rDiagonal)
    {
        std::cout << "Left" << std::endl;
    }
    else if (!lDiagonal && rDiagonal)
    {
        std::cout << "Right" << std::endl;
    }
    else
    {
        std::cout << "Below" << std::endl;
    }
    
}


