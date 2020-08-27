#include "common.h"
#include "cmath"
#include "string"
#include "window.h"

class Sphere{
    public:
        int centerx, centery, centerz, radius;

        win &gmwin;
        Sphere(win &gmwin, int centerx, int centery, int centerz, int radius);
        double SignedDistToSphere(double ox, double oy, double oz);
        void RenderSphere(int offx, int offy);
    private:
};