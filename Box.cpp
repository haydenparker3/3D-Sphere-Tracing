#include "common.h"
#include "cmath"
#include "string"
#include "Box.h"

Box :: Box(win &gmwin, double centerx, double centery, double centerz, double width, double height, double length, SDL_Color color) : Shape(gmwin, centerx, centery, centerz, color), width(width), height(height), length(length){

}

void Box :: RenderShape(int offx, int offy){
    gmwin.DrawRectangle(true, centerx, centery, width, height, {0, 0, 0, 255}, offx, offy);
}

double Box :: SignedDistToShape(double ox, double oy, double oz){
    double x = abs(ox - centerx) - width/2;
    double y = abs(oy - centery) - height/2;
    double z = abs(oz - centerz) - length/2;
    double unsignedDist = sqrt(pow(max(x, 0.0), 2) + pow(max(y, 0.0), 2) + pow(max(z, 0.0), 2));
    
    double dstInsideBox = max(max(min(x, 0.0), min(y, 0.0)), min(z, 0.0));
    return unsignedDist + dstInsideBox;
    // double dx = max(abs(ox - centerx) - width / 2, 0.0);
    // double dy = max(abs(oy - centery) - height / 2, 0.0);
    // return sqrt(dx * dx + dy * dy);
}
