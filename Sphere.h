#pragma one
#ifndef SQPERE_H
#define SQHERE_H
#include "common.h"
#include "cmath"
#include "string"
#include "window.h"
#include "Shape.h"

class Sphere : public Shape{
    public:
        int radius;

        Sphere(win &gmwin, double centerx, double centery, double centerz, int radius, SDL_Color color);
        double SignedDistToShape(double ox, double oy, double oz);
        void RenderShape(int offx, int offy);
    private:
};
#endif