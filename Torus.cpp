#include "common.h"
#include "cmath"
#include "string"
#include "Torus.h"

Torus :: Torus(win &gmwin, double centerx, double centery, double centerz, double r1, double r2, SDL_Color color) : Shape(gmwin, centerx, centery, centerz, color), r1(r1), r2(r2){

}

double Torus :: SignedDistToShape(double ox, double oy, double oz){
    double qx = sqrt(pow(ox-centerx, 2) + pow(oz-centerz, 2)) - r1;
    double qy = oy - centery;
    return sqrt(pow(qx, 2) + pow(qy, 2)) - r2;
}
