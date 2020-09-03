#pragma once
#ifndef SHAPE_H
#define SHAPE_H
#include "common.h"
#include "cmath"
#include "string"
#include "window.h"

class Shape{
    public:
        double centerx, centery, centerz;
        SDL_Color color;

        win &gmwin;
        Shape(win &gmwin, double centerx, double centery, double centerz, SDL_Color color);
        virtual double SignedDistToShape(double ox, double oy, double oz){return 0;}
        virtual void RenderShape(int offx, int offy){}
    private:
};
#endif