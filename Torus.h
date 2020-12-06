#pragma one
#ifndef TORUS_H
#define TORUS_H
#include "common.h"
#include "cmath"
#include "string"
#include "window.h"
#include "Shape.h"

class Torus : public Shape{
    public:
        double r1;
        double r2;

        Torus(win &gmwin, double centerx, double centery, double centerz, double r1, double r2, SDL_Color color);
        double SignedDistToShape(double ox, double oy, double oz);
    private:
};
#endif