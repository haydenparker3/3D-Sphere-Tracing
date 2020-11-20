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
void RenderTrace(win &gmwin);
void RenderMandelTrace(win &gmwin);
vector<double> sphereTracing(Vector3 *rayVector);
vector<double> mandelSphereTracing(Vector3 *rayVector);
double smoothMin(double a, double b, double k);
double pointInShadow(double x, double y, double z);
double distMandelBulb(double x, double y, double z);
double mix(double x, double y, double a);
double clamp(double x, double min, double max);

int mx, my, pmx, pmy;
double ox = 0; //camerax
double oy = 0; //cameray
double oz = 301; //camerz
double pox, poy, poz; //previous cameraxyz
double minDist;
Vector3 cameraVector = Vector3(0, 0, -1, ox, oy, oz);
Vector3 cameraUpVector = Vector3(0, 1, 0, ox, oy, oz);
double zoom = .6;
double viewRange = M_PI/2;
int resolution = 4;
int xrays = 600/resolution;
int yrays = 600/resolution;
int step = 150; //shape movement step
bool w, s, a, d, l, r;
int m = 0; //shape your controling
double k = 80; //smoothMin amplitude
Vector3 lightVector = Vector3(0, 1, 0, 0, 0, 0); //light vector
double lax, lay, laz; //angle light comes from
double ud = 1; //point in shadow initial step size
double theta = 3*M_PI/2; 
double power = 2; //used in mandelbulb
double mult = 13; //multiplier in rendering

vector<Shape*> shapes;
vector<vector<vector<double>>> minDistances;
vector<vector<vector<double>>> pminDistances; //previous distances

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

    vector<vector<double>> temp(yrays, {10000, 1}); //distance and in shadow
    for(int i = 0; i < xrays; i++){
        minDistances.push_back(temp);
    }
    pminDistances = minDistances;

    shapes.push_back(new Sphere(gmwin, 0, 0, 0, 75, {255, 0, 0, 255}));
    shapes.push_back(new Box(gmwin, -100, 0, 0, 100, 100, 100, {0, 255, 0, 255}));

    auto start = chrono::system_clock::now();
    auto end = chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds;
    
    while (gameLoop)
    {   
        start = chrono::system_clock::now();

        SDL_RenderPresent(gmwin.renderer);
        gmwin.ClearWindow();

        // pmx = mx;
        // pmy = my;
        // SDL_GetMouseState(&mx, &my);

        cameraVector.i = cos(theta);
        cameraVector.k = sin(theta);
        
        if(w){
            if(m == 0){
                shapes[m]->centery += step*elapsed_seconds.count();
            }
            if(m == 1){
                shapes[m]->centery += step*elapsed_seconds.count();
            }
            if(m == 2 || m == 3){
                pox = cameraVector.x;
                poy = cameraVector.y;
                poz = cameraVector.z;
                cameraVector.x += step*elapsed_seconds.count()*cameraVector.i;
                cameraVector.y += step*elapsed_seconds.count()*cameraVector.j;
                cameraVector.z += step*elapsed_seconds.count()*cameraVector.k;
                // if(calcMinDist(cameraVector.x, cameraVector.y, cameraVector.z).first < .01){
                //     cout << "WOW" << endl;
                //     cameraVector.x = pox;
                //     cameraVector.y = poy;
                //     cameraVector.z = poz;
                // }
            }
        }
        if(s){
            if(m == 0){
                shapes[m]->centery -= step*elapsed_seconds.count();
            }
            else if(m == 1){
                shapes[m]->centery -= step*elapsed_seconds.count();
            }
            else if(m == 2 || m == 3){
                pox = cameraVector.x;
                poy = cameraVector.y;
                poz = cameraVector.z;
                cameraVector.x -= step*elapsed_seconds.count()*cameraVector.i;
                cameraVector.y -= step*elapsed_seconds.count()*cameraVector.j;
                cameraVector.z -= step*elapsed_seconds.count()*cameraVector.k;
                // if(calcMinDist(cameraVector.x, cameraVector.y, cameraVector.z).first < .01){
                //     cout << "WOW" << endl;
                //     cameraVector.x = pox;
                //     cameraVector.y = poy;
                //     cameraVector.z = poz;
                // }
            }
        }
        if(a){
            if(m == 0){
                shapes[m]->centerx -= step*elapsed_seconds.count();
            }
            else if(m == 1){
                shapes[m]->centerx -= step*elapsed_seconds.count();
            }
            else if(m == 2 || m == 3){
                Vector3 camera_right = cameraVector.cross(cameraUpVector);
                pox = cameraVector.x;
                poy = cameraVector.y;
                poz = cameraVector.z;
                cameraVector.x -= step*elapsed_seconds.count()*camera_right.i;
                cameraVector.y -= step*elapsed_seconds.count()*camera_right.j;
                cameraVector.z -= step*elapsed_seconds.count()*camera_right.k;
                // if(calcMinDist(cameraVector.x, cameraVector.y, cameraVector.z).first < .01){
                //     cameraVector.x = pox;
                //     cameraVector.y = poy;
                //     cameraVector.z = poz;
                // }
            }
        }
        if(d){
            if(m == 0){
                shapes[m]->centerx += step*elapsed_seconds.count();
            }
            else if(m == 1){
                shapes[m]->centerx += step*elapsed_seconds.count();
            }
            else if(m == 2 || m == 3){
                Vector3 camera_right = cameraVector.cross(cameraUpVector);
                pox = cameraVector.x;
                poy = cameraVector.y;
                poz = cameraVector.z;
                cameraVector.x += step*elapsed_seconds.count()*camera_right.i;
                cameraVector.y += step*elapsed_seconds.count()*camera_right.j;
                cameraVector.z += step*elapsed_seconds.count()*camera_right.k;
                // if(calcMinDist(cameraVector.x, cameraVector.y, cameraVector.z).first < .01){
                //     cameraVector.x = pox;
                //     cameraVector.y = poy;
                //     cameraVector.z = poz;
                // }
            }
        }   

        if(m!=3)
            RenderTrace(gmwin);
        else{
            power+=.05;
            RenderMandelTrace(gmwin);
        }

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
                    case SDLK_e:
                        zoom += .1;
                        break;
                    case SDLK_q:
                        zoom -= .1;
                        break;
                    case SDLK_RIGHT:
                        theta += (M_PI/4)*elapsed_seconds.count();
                        break;
                    case SDLK_LEFT:
                        theta -= (M_PI/4)*elapsed_seconds.count();
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
                        else if(m == 2){
                            step = 1;
                            power = 2;
                            cameraVector.i = 0;
                            cameraVector.j = 0;
                            cameraVector.k = -1;
                            cameraVector.x = 0;
                            cameraVector.y = 0;
                            cameraVector.z = 2.5;
                            m = 3;
                        }
                        else if(m == 3){
                            step = 150;
                            cameraVector.i = 0;
                            cameraVector.j = 0;
                            cameraVector.k = -1;
                            cameraVector.x = 0;
                            cameraVector.y = 0;
                            cameraVector.z = -300;
                            m = 0;
                        }
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
    //     if(circ.SignedDistToSphere(cameraVector.x, cameraVector.y, cameraVector.z) < minDist)
    //         minDist = circ.SignedDistToSphere(cameraVector.x, cameraVector.y, cameraVector.z);
    // }
    // for(Box &sq : boxes){
    //     if(sq.SignedDistToBox(cameraVector.x, cameraVector.y, cameraVector.z) < minDist)
    //         minDist = sq.SignedDistToBox(cameraVector.x, cameraVector.y, cameraVector.z);
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

void RenderTrace(win &gmwin){
    Vector3 camera_right = cameraUpVector.cross(cameraVector);
    Vector3 camera_direction = cameraVector*zoom;
    pminDistances = minDistances;
    for(int ix = 0; ix < xrays; ix++){
        for(int iy = 0; iy < yrays; iy++){
            double width = xrays;  // pixels across
            double height = yrays;  // pixels high
            double normalized_i = xrays/width/2 - (ix / width);
            double normalized_j = yrays/height/2 - (iy / height);
            
            double imagepointx = (camera_right * normalized_i).i + (cameraUpVector * normalized_j).i + camera_direction.x + camera_direction.i;
            double imagepointy = (camera_right * normalized_i).j + (cameraUpVector * normalized_j).j + camera_direction.y + camera_direction.j;
            double imagepointz = (camera_right * normalized_i).k + (cameraUpVector * normalized_j).k + camera_direction.z + camera_direction.k;
            Vector3 rayVector = Vector3(imagepointx - camera_direction.x, imagepointy - camera_direction.y, imagepointz - camera_direction.z, camera_direction.x, camera_direction.y, camera_direction.z);
            
            minDistances[ix][iy] = sphereTracing(&rayVector);
        }
    }
    
    for(int x = 0; x < minDistances.size(); x++){
        for(int y = 0; y < minDistances[x].size(); y++){
            if(minDistances[x][y][0] == 10000 && pminDistances[x][y][0] != 10000){
                SDL_SetRenderDrawColor(gmwin.renderer, 0, 0, 0, 255);
                gmwin.pos.x = x*resolution;
                gmwin.pos.y = y*resolution;
                gmwin.pos.h = resolution;
                gmwin.pos.w = resolution;
                SDL_RenderFillRect(gmwin.renderer, &gmwin.pos);
            }
            else{
                if(minDistances[x][y][1] == 1){ // 1 means in shadow
                    SDL_SetRenderDrawColor(gmwin.renderer, minDistances[x][y][2]/2, minDistances[x][y][3]/2, minDistances[x][y][4]/2, 255);
                }
                else if(minDistances[x][y][1] == 0){ //0 means in light so no change
                    SDL_SetRenderDrawColor(gmwin.renderer, minDistances[x][y][2], minDistances[x][y][3], minDistances[x][y][4], 255);
                }
                gmwin.pos.x = x*resolution;
                gmwin.pos.y = y*resolution;
                gmwin.pos.h = resolution;
                gmwin.pos.w = resolution;
                SDL_RenderFillRect(gmwin.renderer, &gmwin.pos);
            }
            //Alternate Render
            // SDL_SetRenderDrawColor(gmwin.renderer, (-1/(minDistances[x][y][5]/mult+1)+1)*255, (-1/(minDistances[x][y][5]/mult+1)+1)*255, (-1/(minDistances[x][y][5]/mult+1)+1)*255, 255);
            
            // gmwin.pos.x = x*resolution;
            // gmwin.pos.y = y*resolution;
            // gmwin.pos.h = resolution;
            // gmwin.pos.w = resolution;
            // SDL_RenderFillRect(gmwin.renderer, &gmwin.pos);
        }
    }
}

void RenderMandelTrace(win &gmwin){
    Vector3 camera_right = cameraUpVector.cross(cameraVector);
    Vector3 camera_direction = cameraVector*zoom;
    for(int ix = 0; ix < xrays; ix++){
        for(int iy = 0; iy < yrays; iy++){
            double width = xrays;  // pixels across
            double height = yrays;  // pixels high
            double normalized_i = xrays/width/2 - (ix / width);
            double normalized_j = yrays/height/2 - (iy / height);

            double imagepointx = (camera_right * normalized_i).i + (cameraUpVector * normalized_j).i + camera_direction.x + camera_direction.i;
            double imagepointy = (camera_right * normalized_i).j + (cameraUpVector * normalized_j).j + camera_direction.y + camera_direction.j;
            double imagepointz = (camera_right * normalized_i).k + (cameraUpVector * normalized_j).k + camera_direction.z + camera_direction.k;
            Vector3 rayVector = Vector3(imagepointx - camera_direction.x, imagepointy - camera_direction.y, imagepointz - camera_direction.z, camera_direction.x, camera_direction.y, camera_direction.z);
            
            minDistances[ix][iy] = mandelSphereTracing(&rayVector);
        }
    }
    
    for(int x = 0; x < minDistances.size(); x++){
        for(int y = 0; y < minDistances[x].size(); y++){
            SDL_SetRenderDrawColor(gmwin.renderer, (-1/(minDistances[x][y][1]/mult+1)+1)*255, 0, (-1/(minDistances[x][y][1]/mult+1)+1)*255*.60, 255);

            gmwin.pos.x = x*resolution;
            gmwin.pos.y = y*resolution;
            gmwin.pos.h = resolution;
            gmwin.pos.w = resolution;
            SDL_RenderFillRect(gmwin.renderer, &gmwin.pos);
        }
    }
}

vector<double> sphereTracing(Vector3 *rayVector){
    pair<double, Shape*> minDist = calcMinDist(cameraVector.x, cameraVector.y, cameraVector.z);
    vector<double> angs = rayVector->getAngles();
    double stepox = cameraVector.x;
    double stepoy = cameraVector.y;
    double stepoz = cameraVector.z;
    double pstepox = cameraVector.x;
    double pstepoy = cameraVector.y;
    double psteooz = cameraVector.z;
    double iters = 0;
    
    while(minDist.first > .1 && minDist.first < 2000){
        iters++;
        stepox = stepox + minDist.first*cos(angs[0]);
        stepoy = stepoy + minDist.first*cos(angs[1]);
        stepoz = stepoz + minDist.first*cos(angs[2]);
        minDist = calcMinDist(stepox, stepoy, stepoz);
    }
     
    if(minDist.first <= .1){
        return {sqrt(pow(stepox-cameraVector.x, 2) + pow(stepoy-cameraVector.y, 2) + pow(stepoz-cameraVector.z, 2)), pointInShadow(stepox, stepoy, stepoz), static_cast<double>(minDist.second->color.r), static_cast<double>(minDist.second->color.g), static_cast<double>(minDist.second->color.b), iters};
    }
    else{
        return {10000, 1, 0, 0, 0, iters}; //distance shadow r g b iters
    }
    rayVector = nullptr;
    delete rayVector;
}

vector<double> mandelSphereTracing(Vector3 *rayVector){
    double minDist = distMandelBulb(cameraVector.x, cameraVector.y, cameraVector.z);
    vector<double> angs = rayVector->getAngles();
    double stepox = cameraVector.x;
    double stepoy = cameraVector.y;
    double stepoz = cameraVector.z;
    double pstepox = cameraVector.x;
    double pstepoy = cameraVector.y;
    double psteooz = cameraVector.z;
    double iters = 0;
    while(minDist > .01 && minDist < 2000){
        iters++;
        stepox = stepox + minDist*cos(angs[0]);
        stepoy = stepoy + minDist*cos(angs[1]);
        stepoz = stepoz + minDist*cos(angs[2]);
        minDist = distMandelBulb(stepox, stepoy, stepoz);
    }

    if(minDist <= .01){
        return {sqrt(pow(stepox-cameraVector.x, 2) + pow(stepoy-cameraVector.y, 2) + pow(stepoz-cameraVector.z, 2)), iters};
    }
    else{
        return {10000, iters};
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

    vector<double> angs = lightVector.getAngles();
    stepox = stepox + ud*cos(angs[0]);
    stepoy = stepoy + ud*cos(angs[1]);
    stepoz = stepoz + ud*cos(angs[2]);
    minDist = calcMinDist(stepox, stepoy, stepoz);
    if(minDist.first <= .1){
        return 1;
    }
    while(minDist.first > .1 && minDist.first < 2000){
        stepox = stepox + minDist.first*cos(angs[0]);
        stepoy = stepoy + minDist.first*cos(angs[1]);
        stepoz = stepoz + minDist.first*cos(angs[2]);
        minDist = calcMinDist(stepox, stepoy, stepoz);
    }
    
    if(minDist.first <= .1){
        return 1;
    }
    else{
        return 0;
    }
}

double distMandelBulb(double x, double y, double z){
    double zx = x;
    double zy = y;
    double zz = z;
    double dr = 1;
    double r;

    for(int i = 0; i < 15; i++){
        r = sqrt(zx*zx + zy*zy + zz*zz);
        if(r > 2)
            break;
        
        double theta = acos(zz / r ) * power;
        double phi = atan2(zy, zx) * power;
        double zr = pow(r, power);
        dr = pow(r, power - 1) * power * dr + 1;

        zx = zr*sin(theta) * cos(phi);
        zy = zr*sin(phi) * sin(theta);
        zz = zr*cos(theta);
        zx += x;
        zy += y;
        zz += z;
    }
    return .5 * log(r) * r/dr;
}

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