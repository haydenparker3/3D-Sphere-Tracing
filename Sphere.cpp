#include "common.h"
#include "cmath"
#include "string"
#include "Sphere.h"

Sphere :: Sphere(win &gmwin, int centerx, int centery, int centerz, int radius) : gmwin(gmwin), centerx(centerx), centery(centery), centerz(centerz), radius(radius){

}

double Sphere :: SignedDistToSphere(double ox, double oy, double oz){
    return sqrt(pow(centerx - ox, 2) + pow(centery - oy, 2) + pow(centery - oz, 2)) - radius; //negative if origin is in circle
}

void Sphere :: RenderSphere(int offx, int offy){
    gmwin.DrawCircle(true, centerx, centery, radius, {0, 0, 0, 155}, offx, offy);
}
