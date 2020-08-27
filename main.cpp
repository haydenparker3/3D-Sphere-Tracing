#include "common.h"
#include "cmath"
#include "string"
#include "vector"
#include "window.h"
#include "Box.h"
#include "Sphere.h"
#include "chrono"

void Run(win &gmwin);
double calcMinDist(double ox, double oy, double oz);
void renderShapes();
void RenderTrace(win &gmwin);
double sphereTracing(bool draw, win &gmwin, double angx, double angy, double angz);
double smoothMin(double a, double b, double k);
// double distMandelBulb(double x, double y, double z);
double mix(double x, double y, double a);
double clamp(double x, double min, double max);

int mx, my, pmx, pmy;
double ox, oy, oz, pox, poy, poz;
double minDist;
double viewAngx = 0;
double viewAngy = 0;
double viewAngz = 0;
double viewRange = M_PI/2;
int resolution = 2;
int xrays = 600/resolution;
int yrays = 600/resolution;
int step = 150;
bool w, s, a, d, l, r;
int m = 0;
double k = .1;

vector<Sphere> spheres;
vector<Box> boxes;
vector<vector<double>> minDistances;

int main()
{
    win gmwin = win(600, 600);

    Run(gmwin);

    gmwin.~win();
    return 0;
}

void Run(win &gmwin)
{
    bool gameLoop = true;
    srand(time(NULL));
    cout << std::boolalpha;

    vector<double> temp(yrays, 10000);
    for(int i = 0; i < xrays; i++){
        minDistances.push_back(temp);
    }

    spheres.push_back(Sphere(gmwin, 0, 0, 0, 75));
    boxes.push_back(Box(gmwin, -100, 0, 0, 100, 100, 100));
    // circles.push_back(Circle(gmwin, 400, 100, 50));
    // rectangles.push_back(Rectangle(gmwin, 60, 440, 50, 50));
    ox = 0;
    oy = 0;
    oz = -300;

    auto start = chrono::system_clock::now();
    auto end = chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds;

    double cx = 1;
    double cy = 1;
    double cz = 1;
    double ax = atan2(sqrt(cy*cy+cz*cz), cx); //tan^-1(y/x) = angle sqrt(cy*cy+cz*cz) = y cx = x minDist*sin(ax) = y
    double ay = atan2(sqrt(cz*cz+cx*cx), cy);
    double az = atan2(sqrt(cx*cx+cy*cy), cz);
    // cout << ax << "\t" << ay << "\t" << az << endl;
    // cout << sqrt(1+1+1)*cos(ax) << endl;

    while (gameLoop)
    {   
        start = chrono::system_clock::now();

        SDL_RenderPresent(gmwin.renderer);
        gmwin.ClearWindow();

        pmx = mx;
        pmy = my;
        SDL_GetMouseState(&mx, &my);
        k = 80;
        // if(l){
        //     viewAng -= (2*M_PI)/4*elapsed_seconds.count();
        //     if(viewAng < 0)
        //         viewAng += 2*M_PI;
        // }
        // if(r){
        //     viewAng += (2*M_PI)/4*elapsed_seconds.count();
        //     if(viewAng >= 2*M_PI)
        //         viewAng -= 2*M_PI;
        // }
        if(w){
            if(m == 0)
                boxes[0].centery -= step*elapsed_seconds.count();
            if(m == 1)
                spheres[0].centery -= step*elapsed_seconds.count();
            if(m == 2)
                oz += step*elapsed_seconds.count();
            // ox += cos(viewAng)*step*elapsed_seconds.count();
            // oy += sin(viewAng)*step*elapsed_seconds.count();
        }
        if(s){
            if(m == 0)
                boxes[0].centery += step*elapsed_seconds.count();
            if(m == 1)
                spheres[0].centery += step*elapsed_seconds.count();
            if(m == 2)
                oz -= step*elapsed_seconds.count();
            // ox -= cos(viewAng)*step*elapsed_seconds.count();
            // oy -= sin(viewAng)*step*elapsed_seconds.count();
        }
        if(a){
            if(m == 0)
                boxes[0].centerx -= step*elapsed_seconds.count();
            if(m == 1)
                spheres[0].centerx -= step*elapsed_seconds.count();
            if(m == 2)
                ox -= step*elapsed_seconds.count();
            // ox += cos(viewAng-M_PI/2)*step*elapsed_seconds.count();
            // oy += sin(viewAng-M_PI/2)*step*elapsed_seconds.count();
        }
        if(d){
            if(m == 0)
                boxes[0].centerx += step*elapsed_seconds.count();
            if(m == 1)
                spheres[0].centerx += step*elapsed_seconds.count();
            if(m == 2)
                ox += step*elapsed_seconds.count();
            // ox += cos(viewAng+M_PI/2)*step*elapsed_seconds.count();
            // oy += sin(viewAng+M_PI/2)*step*elapsed_seconds.count();
        }

        RenderTrace(gmwin);

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                gameLoop = false;
            }
            if(event.type == SDL_MOUSEBUTTONDOWN){
                if(event.button.button == SDL_BUTTON_LEFT){
                    
                }
                else if(event.button.button == SDL_BUTTON_RIGHT){
                    
                }
            }
            if(event.type == SDL_MOUSEBUTTONUP){
                if(event.button.button == SDL_BUTTON_LEFT){
                    
                }
            }
            if (event.type == SDL_KEYDOWN)
            {
                switch (event.key.keysym.sym){
                    case SDLK_ESCAPE:
                        gameLoop = false;
                        break;
                    case SDLK_LEFT:
                        l = true;
                        break;
                    case SDLK_RIGHT:
                        r = true;
                        break;
                    case SDLK_w:
                        w = true;
                        break;
                    case SDLK_a:
                        a = true;
                        break;
                    case SDLK_s:
                        s = true;
                        break;
                    case SDLK_d:
                        d = true;
                        break;
                    case SDLK_SPACE:
                        if(m == 0)
                            m = 1;
                        else if(m == 1)
                            m = 2;
                        else if(m == 2)
                            m = 0;
                        break;
                    default:
                        break;
                }
            }

            if (event.type == SDL_KEYUP)
            {
                switch (event.key.keysym.sym){
                    case SDLK_LEFT:
                        l = false;
                        break;
                    case SDLK_RIGHT:
                        r = false;
                        break;
                    case SDLK_w:
                        w = false;
                        break;
                    case SDLK_a:
                        a = false;
                        break;
                    case SDLK_s:
                        s = false;
                        break;
                    case SDLK_d:
                        d = false;
                        break;
                    default:
                        break;
                }
            }
        }
        end = chrono::system_clock::now();
        elapsed_seconds = end-start;
    }
}

double calcMinDist(double ox, double oy, double oz){
    // double minDist = 10000;
    // for(Sphere &circ : spheres){
    //     if(circ.SignedDistToSphere(ox, oy, oz) < minDist)
    //         minDist = circ.SignedDistToSphere(ox, oy, oz);
    // }
    // for(Box &sq : boxes){
    //     if(sq.SignedDistToBox(ox, oy, oz) < minDist)
    //         minDist = sq.SignedDistToBox(ox, oy, oz);
    // }

    // return minDist;
    double dstToBox = boxes[0].SignedDistToBox(ox, oy, oz);
    double dstToSphere = spheres[0].SignedDistToSphere(ox, oy, oz);

    return smoothMin(dstToBox, dstToSphere, k);
}

void renderShapes(){
    for(Sphere &circ : spheres){
        circ.RenderSphere(0, 0);
    }
    for(Box &sq : boxes){
        sq.RenderBox(0, 0);
    }
}

void RenderTrace(win &gmwin){
    double z = oz + 1;
    int ix = 0;
    int iy = 0;
    for(double x = ox-1; x <= ox+1-2.0/xrays; x+=2.0/xrays){
        iy = 0;
        for(double y = oy-1; y <= oy+1-2.0/yrays; y+=2.0/yrays){
            double cx = x - ox;
            double cy = y - oy;
            double cz = z - oz;
            double ax = atan2(sqrt(cy*cy+cz*cz), cx);
            double ay = atan2(sqrt(cz*cz+cx*cx), cy);
            double az = atan2(sqrt(cx*cx+cy*cy), cz);
            minDistances[ix][iy] = sphereTracing(false, gmwin, ax, ay, az);
            iy++;
        }
        ix++;
    }
    
    for(int x = 0; x < minDistances.size(); x++){
        for(int y = 0; y < minDistances[x].size(); y++){
            if(minDistances[x][y] == 10000){
                
            }
            else{
                SDL_SetRenderDrawColor(gmwin.renderer, clamp(-.5*minDistances[x][y]+255, 0, 200), 0, 0, 255);
                gmwin.pos.x = x*resolution;
                gmwin.pos.y = y*resolution;
                gmwin.pos.h = resolution;
                gmwin.pos.w = resolution;
                SDL_RenderFillRect(gmwin.renderer, &gmwin.pos);
            }
        }
    }
}

double sphereTracing(bool draw, win &gmwin, double angx, double angy, double angz){
    double minDist = calcMinDist(ox, oy, oz);
    double stepox = ox;
    double stepoy = oy;
    double stepoz = oz;
    while(stepox >= -500 && stepox <= 500 && stepoy >= -500 && stepoy <= 500 && stepoz >= -500 && stepoz <= 500 && minDist > .01){
        minDist = calcMinDist(stepox, stepoy, stepoz);
        stepox = stepox + minDist*cos(angx);
        stepoy = stepoy + minDist*cos(angy);
        stepoz = stepoz + minDist*cos(angz);
    }
    if(minDist <= .01)
        return sqrt(pow(stepox-ox, 2) + pow(stepoy-oy, 2) + pow(stepoz-oz, 2));
    else
        return 10000;
}

double smoothMin(double a, double b, double k){
    // double h = max(k-abs(a-b), 0.0) / k;
    // if(h != 0){
    //     cout << k << "\t" << h << endl;
    //     cout << a << "\t" << b << "\t" << min(a, b) - h*h*h*k*1/6.0 << endl;
    // }
    // return min(a, b) - h*h*h*k*1/6.0;

    double h = clamp(.5 + .5 *(b-a)/k, 0.0, 1.0);
    // if(a != mix(b, a, h) - k*h*(1.0-h) && b != mix(b, a, h) - k*h*(1.0-h))
    //     cout << a << "\t" << b << "\t" << mix(b, a, h) - k*h*(1.0-h) << endl;
    return mix(b, a, h) - k*h*(1.0-h);
}

// double distMandelBulb(double x, double y, double z){
//     double zx = x;
//     double zy = y;
//     double zz = z;
//     double dr = 1;
//     double r;
//     double power = 8;

//     for(int i = 0; i < 15; i++){
//         r = sqrt(zx*zx + zy*zy + zz*zz);
//         if(r > 2)
//             break;
        
//         double theta = acos(zz / r ) * power;
//         double phi = atan2(zy, zx) * power;
//         double zr = pow(r, power);
//         dr = pow(r, power - 1) * power * dr + 1;

//         zx = zr*sin(theta) * cos(phi);
//         zy = zr*sin(phi) * sin(theta);
//         zz = zr*cos(theta);
//         zx += x;
//         zy += y;
//         zz += z;
//     }
//     return .5 * log(r) * r/dr;
// }

double mix(double x, double y, double a){
    return x*(1-a) + y*a;
}

double clamp(double x, double min, double max){
    if(x < min)
        x = min;
    else if(x > max)
        x = max;
    
    return x;
}