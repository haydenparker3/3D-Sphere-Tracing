#include "iostream"
#include "common.h"
#include "cmath"
#include "string"
#include "vector"
#include "window.h"
#include "Box.h"
#include "Sphere.h"
#include "Vector3D.h"
#include "chrono"

void Run(win &gmwin);
pair<double, Shape*> calcMinDist(double x, double y, double z);
void renderShapes();
void RenderTrace(win &gmwin);
vector<double> sphereTracing(double angx, double angy, double angz);
double smoothMin(double a, double b, double k);
double pointInShadow(double x, double y, double z);
// double distMandelBulb(double x, double y, double z);
double mix(double x, double y, double a);
double clamp(double x, double min, double max);

int mx, my, pmx, pmy;
double ox = 0;
double oy = 0;
double oz = -300;
double pox, poy, poz;
double minDist;
Vector3 cameraVector = Vector3(0, 0, 1);
Vector3 cameraUpVector = Vector3(0, 1, 0);
double viewRange = M_PI/2;
int resolution = 3;
int xrays = 600/resolution;
int yrays = 600/resolution;
int step = 150;
bool w, s, a, d, l, r;
int m = 0;
double k = 80; //smoothMin amplitude
Vector3 lightVector = Vector3(0, 1, 0); //light vector
double lax, lay, laz; //angle light comes from
double ud = 1;

vector<Shape*> shapes;
vector<vector<vector<double>>> minDistances;

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


    vector<vector<double>> temp(yrays, {10000, 1});
    for(int i = 0; i < xrays; i++){
        minDistances.push_back(temp);
    }

    shapes.push_back(new Sphere(gmwin, 0, 0, 0, 75, {255, 0, 0, 255}));
    shapes.push_back(new Box(gmwin, -100, 0, 0, 100, 100, 100, {0, 255, 0, 255}));

    auto start = chrono::system_clock::now();
    auto end = chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds;

    vector<double> angs = lightVector.getAngles();
    lax = angs[0];
    lay = angs[1];
    laz = angs[2];
    cout << lax << "\t" << lay << "\t" << laz << endl;


    Vector3 cross = cameraUpVector.cross(cameraVector);
    cout << cross.i << "\t" << cross.j << "\t" << cross.k << endl;
    
    while (gameLoop)
    {   
        start = chrono::system_clock::now();

        SDL_RenderPresent(gmwin.renderer);
        gmwin.ClearWindow();

        pmx = mx;
        pmy = my;
        SDL_GetMouseState(&mx, &my);
        
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
            if(m == 0){
                shapes[m]->centery += step*elapsed_seconds.count();
            }
            if(m == 1){
                shapes[m]->centery += step*elapsed_seconds.count();
            }
            if(m == 2){
                poz = oz;
                oz += step*elapsed_seconds.count();
                if(calcMinDist(ox, oy, oz).first < .01){
                    oz = poz;
                }
            }
            
            // ox += cos(viewAng)*step*elapsed_seconds.count();
            // oy += sin(viewAng)*step*elapsed_seconds.count();
        }
        if(s){
            if(m == 0){
                shapes[m]->centery -= step*elapsed_seconds.count();
            }
            else if(m == 1){
                shapes[m]->centery -= step*elapsed_seconds.count();
            }
            else if(m == 2){
                poz = oz;
                oz -= step*elapsed_seconds.count();
                if(calcMinDist(ox, oy, oz).first < .01){
                    oz = poz;
                }
            }
            // ox -= cos(viewAng)*step*elapsed_seconds.count();
            // oy -= sin(viewAng)*step*elapsed_seconds.count();
        }
        if(a){
            if(m == 0){
                shapes[m]->centerx -= step*elapsed_seconds.count();
            }
            else if(m == 1){
                shapes[m]->centerx -= step*elapsed_seconds.count();
            }
            else if(m == 2){
                pox = ox;
                ox -= step*elapsed_seconds.count();
                if(calcMinDist(ox, oy, oz).first < .01){
                    ox = pox;
                }
            }
            // ox += cos(viewAng-M_PI/2)*step*elapsed_seconds.count();
            // oy += sin(viewAng-M_PI/2)*step*elapsed_seconds.count();
        }
        if(d){
            if(m == 0){
                shapes[m]->centerx += step*elapsed_seconds.count();
            }
            else if(m == 1){
                shapes[m]->centerx += step*elapsed_seconds.count();
            }
            else if(m == 2){
                pox = ox;
                ox += step*elapsed_seconds.count();
                if(calcMinDist(ox, oy, oz).first < .01){
                    ox = pox;
                }
            }
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
                    case SDLK_UP:
                        ud += .01;
                        cout << ud << endl;
                        break;
                    case SDLK_DOWN:
                        ud -= .01;
                        cout << ud << endl;
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

pair<double, Shape*> calcMinDist(double x, double y, double z){
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
    double dstToBox = shapes[1]->SignedDistToShape(x, y, z);
    double dstToSphere = shapes[0]->SignedDistToShape(x, y, z);
    Shape* nearestShape;
    if(dstToBox < dstToSphere)
        nearestShape = shapes[1];
    else
        nearestShape = shapes[0];

    return {smoothMin(dstToBox, dstToSphere, k), nearestShape};
}

void renderShapes(){
    for(Shape* sh : shapes){
        sh->RenderShape(0, 0);
    }
}

void RenderTrace(win &gmwin){
    double z = oz + 1;
    int ix = 0;
    int iy = 0;
    for(double x = ox-1; x <= ox+1; x+=2.0/xrays){
        iy = 0;
        for(double y = oy+1; y >= oy-1; y-=2.0/yrays){
            double cx = x - ox;
            double cy = y - oy;
            double cz = z - oz;
            double ax = atan2(sqrt(cy*cy+cz*cz), cx);
            double ay = atan2(sqrt(cz*cz+cx*cx), cy);
            double az = atan2(sqrt(cx*cx+cy*cy), cz);
            
            minDistances[ix][iy] = sphereTracing(ax, ay, az);
            iy++;
        }
        ix++;
    }
    
    for(int x = 0; x < minDistances.size(); x++){
        for(int y = 0; y < minDistances[x].size(); y++){
            if(minDistances[x][y][0] == 10000){
                SDL_SetRenderDrawColor(gmwin.renderer, 0, 0, 0, 255);
                gmwin.pos.x = x*resolution;
                gmwin.pos.y = y*resolution;
                gmwin.pos.h = resolution;
                gmwin.pos.w = resolution;
                SDL_RenderFillRect(gmwin.renderer, &gmwin.pos);
            }
            else{
                if(minDistances[x][y][1] == 0){ // 0 means in shadow
                    SDL_SetRenderDrawColor(gmwin.renderer, minDistances[x][y][2]/2, minDistances[x][y][3]/2, minDistances[x][y][4]/2, 255);
                }
                else if(minDistances[x][y][1] == 1){ //1 means in light so no change
                    SDL_SetRenderDrawColor(gmwin.renderer, minDistances[x][y][2], minDistances[x][y][3], minDistances[x][y][4], 255);
                }
                gmwin.pos.x = x*resolution;
                gmwin.pos.y = y*resolution;
                gmwin.pos.h = resolution;
                gmwin.pos.w = resolution;
                SDL_RenderFillRect(gmwin.renderer, &gmwin.pos);
            }
        }
    }
}

vector<double> sphereTracing(double angx, double angy, double angz){
    pair<double, Shape*> minDist = calcMinDist(ox, oy, oz);
    double stepox = ox;
    double stepoy = oy;
    double stepoz = oz;
    double pstepox = ox;
    double pstepoy = oy;
    double psteooz = oz;
    while(stepox >= -500 && stepox <= 500 && stepoy >= -500 && stepoy <= 500 && stepoz >= -500 && stepoz <= 500 && minDist.first > .1){
        stepox = stepox + minDist.first*cos(angx);
        stepoy = stepoy + minDist.first*cos(angy);
        stepoz = stepoz + minDist.first*cos(angz);
        minDist = calcMinDist(stepox, stepoy, stepoz);
    }
    if(minDist.first <= .1){
        return {sqrt(pow(stepox-ox, 2) + pow(stepoy-oy, 2) + pow(stepoz-oz, 2)), pointInShadow(stepox, stepoy, stepoz), static_cast<double>(minDist.second->color.r), static_cast<double>(minDist.second->color.g), static_cast<double>(minDist.second->color.b)};
    }
    else{
        return {10000, 1};
    }
}

double smoothMin(double a, double b, double k){
    double h = clamp(.5 + .5 *(b-a)/k, 0.0, 1.0);
    return mix(b, a, h) - k*h*(1.0-h);
}

double pointInShadow(double x, double y, double z){
    double stepox = x;
    double stepoy = y;
    double stepoz = z;
    pair<double, Shape*> minDist = calcMinDist(stepox, stepoy, stepoz);

    stepox = stepox + ud*cos(lax);
    stepoy = stepoy + ud*cos(lay);
    stepoz = stepoz + ud*cos(laz);
    minDist = calcMinDist(stepox, stepoy, stepoz);
    if(minDist.first <= .1){
        return 0;
    }
    while(stepox >= -500 && stepox <= 500 && stepoy >= -500 && stepoy <= 500 && stepoz >= -500 && stepoz <= 500 && minDist.first > .1){
        stepox = stepox + minDist.first*cos(lax);
        stepoy = stepoy + minDist.first*cos(lay);
        stepoz = stepoz + minDist.first*cos(laz);
        minDist = calcMinDist(stepox, stepoy, stepoz);
    }
    
    if(minDist.first <= .1){
        return 0;
    }
    else{
        return 1;
    }
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