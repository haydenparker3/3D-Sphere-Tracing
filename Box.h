#include "common.h"
#include "cmath"
#include "string"
#include "window.h"

class Box{
    public:
        int centerx, centery, centerz, width, height, length;

        win &gmwin;
        Box(win &gmwin, int centerx, int centery, int centerz, int width, int height, int length);
        double SignedDistToBox(double ox, double oy, double oz);
        void RenderBox(int offx, int offy);
    private:
};