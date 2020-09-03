#include "common.h"
#include "cmath"
#include "string"
#include "Sphere.h"

Sphere :: Sphere(win &gmwin, double centerx, double centery, double centerz, int radius, SDL_Color color) : Shape(gmwin, centerx, centery, centerz, color), radius(radius){

}

double Sphere :: SignedDistToShape(double ox, double oy, double oz){
    return sqrt(pow(centerx - ox, 2) + pow(centery - oy, 2) + pow(centerz - oz, 2)) - radius; //negative if origin is in circle
}

void Sphere :: RenderShape(int offx, int offy){
    gmwin.DrawCircle(true, centerx, centery, radius, {0, 0, 0, 155}, offx, offy);
}
