#pragma one
#ifndef BOX_H
#define BOX_H
#include "common.h"
#include "cmath"
#include "string"
#include "window.h"
#include "Shape.h"

class Box : public Shape{
    public:
        double width, height, length;

        Box(win &gmwin, double centerx, double centery, double centerz, double width, double height, double length, SDL_Color color);
        double SignedDistToShape(double ox, double oy, double oz);
        void RenderShape(int offx, int offy);
    private:
};
#endif