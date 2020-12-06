#include "iostream"
#include "common.h"
#include "cmath"
#include "string"
#include "vector"
#include "window.h"
#include "Box.h"
#include "Sphere.h"
#include "Torus.h"
#include "Vector3D.h"
#include "chrono"
#include "./threading/mingw.thread.h"

void Run(win &gmwin);
pair<double, Shape*> calcMinDist(double x, double y, double z);
void RenderTrace(win &gmwin);
void RenderTraceWithMultiThreading(win &gmwin);
void RenderMandelTrace(win &gmwin);
void RenderMandelTraceWithMultiThreading(win &gmwin);
vector<double> sphereTracing(Vector3 &rayVector);
void sphereTracingThread(Vector3 camera_direction, Vector3 camera_right, int ix);
vector<double> mandelSphereTracing(Vector3 &rayVector);
void mandelSphereTracingThread(Vector3 camera_direction, Vector3 camera_right, int i);
double smoothMin(double a, double b, double k);
double pointInShadow(double x, double y, double z);
double distMandelBulb(double x, double y, double z);
double mix(double x, double y, double a);
double clamp(double x, double min, double max);
void keyCases(std::chrono::duration<double> elapsed_seconds);

int mx, my, pmx, pmy;
int changemx, changemy;
double ox = 0; //camerax
double oy = 0; //cameray
double oz = 300; //camerz
double pox, poy, poz; //previous cameraxyz
double minDist;
Vector3 cameraVector = Vector3(0, 0, -1, ox, oy, oz);
Vector3 cameraUpVector = Vector3(0, 1, 0, ox, oy, oz);
Vector3 lightVector = Vector3(0, 1, 0, 0, 0, 0); //light vector
double zoom = .6;
double viewRange = M_PI/2;
int resolution = 3;
int screenWidth = 600;
int screenHeight = 600;
bool fullscreen = true;
int xrays = screenWidth/resolution;
int yrays = screenHeight/resolution;
int turnStep = 6;
int step = 200; //shape movement step
bool w, s, a, d, q, e, l, r, up, down;
int m = 0; //shape your controling
double k = 80; //smoothMin amplitude
double ud = 1; //point in shadow initial step size
double power = 2; //used in mandelbulb
double mult = 13; //multiplier in rendering
int threshhold = 7;

vector<Shape*> shapes;
vector<vector<vector<double>>> minDistances;
vector<vector<vector<double>>> pminDistances; //previous distances
vector<thread*> threads;

int main()
{
    win gmwin = win(screenWidth, screenHeight);

    Run(gmwin);

    gmwin.~win();
    return 0;
}

void Run(win &gmwin)
{
    bool gameLoop = true;
    srand(time(NULL));
    cout << std::boolalpha;

    vector<vector<double>> temp(yrays, {10000, 1, 0, 0, 0, 0}); //distance and in shadow
    for(int i = 0; i < xrays; i++){
        minDistances.push_back(temp);
    }
    pminDistances = minDistances;

    shapes.push_back(new Sphere(gmwin, 100, 0, 0, 75, {255, 0, 0, 255}));
    shapes.push_back(new Box(gmwin, -100, 0, 0, 100, 100, 100, {0, 255, 0, 255}));
    shapes.push_back(new Torus(gmwin, 0, 0, 0, 100, 40, {0, 0, 255, 255}));

    auto start = chrono::system_clock::now();
    auto end = chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds;

    // SDL_ShowCursor(SDL_DISABLE);
    // SDL_WarpMouseInWindow(gmwin.window, screenWidth/2, screenHeight/2);
    while (gameLoop)
    {   
        // pmx = mx;
        // pmy = my;
        // SDL_GetMouseState(&mx, &my);
        // changemx = abs(pmx-mx);
        // changemy = abs(pmy-my);

        // SDL_WarpMouseInWindow(gmwin.window, screenWidth/2, screenHeight/2);

        start = chrono::system_clock::now();

        SDL_RenderPresent(gmwin.renderer);
        gmwin.ClearWindow();

        keyCases(elapsed_seconds);

        if(m!=3)
            RenderTraceWithMultiThreading(gmwin);
        else{
            power+=.01;
            RenderMandelTraceWithMultiThreading(gmwin);
        }

        end = chrono::system_clock::now();
        elapsed_seconds = end-start;

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
                    case SDLK_F11:
                        fullscreen = SDL_GetWindowFlags(gmwin.window) & SDL_WINDOW_FULLSCREEN_DESKTOP;
                        SDL_SetWindowFullscreen(gmwin.window, fullscreen ? 0 : SDL_WINDOW_FULLSCREEN_DESKTOP);
                        break;
                    case SDLK_e:
                        e = true;
                        break;
                    case SDLK_q:
                        q = true;
                        break;
                    case SDLK_r:
                        zoom += .1;
                        break;
                    case SDLK_f:
                        zoom -= .1;
                        break;
                    case SDLK_RIGHT:
                        r = true;
                        break;
                    case SDLK_LEFT:
                        l = true;
                        break;
                    case SDLK_UP:
                        up = true;
                        break;
                    case SDLK_DOWN:
                        down = true;
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
                            cameraVector = Vector3(0, 0, -1, 0, 0, 2.5);
                            cameraUpVector = Vector3(0, 1, 0, 0, 0, 2.5);
                            m = 3;
                        }
                        else if(m == 3){
                            step = 150;
                            cameraVector = Vector3(0, 0, -1, 0, 0, 300);
                            cameraUpVector = Vector3(0, 1, 0, 0, 0, 300);
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
                    case SDLK_UP:
                        up = false;
                        break;
                    case SDLK_DOWN:
                        down = false;
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
                    case SDLK_e:
                        e = false;
                        break;
                    case SDLK_q:
                        q = false;
                        break;
                    default:
                        break;
                }
            }
        }
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
    // double dstToBox = shapes[0]->SignedDistToShape(x, y, z);
    // double dstToTorus = shapes[1]->SignedDistToShape(x, y, z);
    // double dstToSphere = shapes[0]->SignedDistToShape(x, y, z);
    Shape* nearestShape = shapes[0];
    for(int i = 1; i < shapes.size(); i++){
        if(shapes[i]->SignedDistToShape(x, y, z) < nearestShape->SignedDistToShape(x, y, z))
            nearestShape = shapes[i];
    }
    double distance = shapes[0]->SignedDistToShape(x, y, z);
    for(int i = 1; i < shapes.size(); i++){
        distance = smoothMin(distance, shapes[i]->SignedDistToShape(x, y, z), k);
    }
    // if(dstToBox < dstToTorus)
    //     nearestShape = shapes[0];
    // else
    //     nearestShape = shapes[1];
    return {distance, nearestShape};
    // return {smoothMin(dstToBox, dstToTorus, k), nearestShape};
    // return {dstToTorus, shapes[0]};
}

void RenderTraceWithMultiThreading(win &gmwin){
    Vector3 camera_right = cameraVector.cross(cameraUpVector);
    Vector3 camera_direction = cameraVector*zoom;
    pminDistances = minDistances;

    for(int ix = 0; ix < xrays/4; ix++){
        threads.push_back(new thread(sphereTracingThread, camera_direction, camera_right, ix));
    }
    for(int i = 0; i < threads.size(); i++){
        threads[i]->join();
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
            // double mu = -1*1/(minDistances[x][y][5]/mult+1+-1*threshhold/mult)+1;
            // if(minDistances[x][y][5] < threshhold)
            //     mu = 0;
            // SDL_SetRenderDrawColor(gmwin.renderer, mu*255, 0, mu*255*.60, 255);

            // gmwin.pos.x = x*resolution;
            // gmwin.pos.y = y*resolution;
            // gmwin.pos.h = resolution;
            // gmwin.pos.w = resolution;
            // SDL_RenderFillRect(gmwin.renderer, &gmwin.pos);
        }
    }
    for(int i = 0; i < threads.size(); i++){
        threads[i] = nullptr;
    }
    threads.clear();
}

void RenderTrace(win &gmwin){
    Vector3 camera_right = cameraVector.cross(cameraUpVector);
    Vector3 camera_direction = cameraVector*zoom;
    pminDistances = minDistances;
    for(int ix = 0; ix < xrays; ix++){
        for(int iy = 0; iy < yrays; iy++){
            double width = xrays;  // pixels across
            double height = yrays;  // pixels high
            double normalized_i = (ix / width) - xrays/width/2;
            double normalized_j = (iy / height) - yrays/height/2;
            
            double imagepointx = (camera_right * normalized_i).i + (cameraUpVector * normalized_j).i + camera_direction.x + camera_direction.i;
            double imagepointy = (camera_right * normalized_i).j + (cameraUpVector * normalized_j).j + camera_direction.y + camera_direction.j;
            double imagepointz = (camera_right * normalized_i).k + (cameraUpVector * normalized_j).k + camera_direction.z + camera_direction.k;
            Vector3 rayVector = Vector3(imagepointx - camera_direction.x, imagepointy - camera_direction.y, imagepointz - camera_direction.z, camera_direction.x, camera_direction.y, camera_direction.z);

            minDistances[ix][yrays-iy-1] = sphereTracing(rayVector);
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

void RenderMandelTraceWithMultiThreading(win &gmwin){
    Vector3 camera_right = cameraVector.cross(cameraUpVector);
    Vector3 camera_direction = cameraVector*zoom;
    pminDistances = minDistances;

    for(int ix = 0; ix < xrays/4; ix++){
        threads.push_back(new thread(mandelSphereTracingThread, camera_direction, camera_right, ix));
    }
    for(int i = 0; i < threads.size(); i++){
        threads[i]->join();
    }
    
    for(int x = 0; x < minDistances.size(); x++){
        for(int y = 0; y < minDistances[x].size(); y++){
            double mu = -1/(minDistances[x][y][1]/mult+1+-1*threshhold/mult)+1;
            if(minDistances[x][y][1] < threshhold)
                mu = 0;
            SDL_SetRenderDrawColor(gmwin.renderer, mu*255, 0, mu*255*.60, 255);

            gmwin.pos.x = x*resolution;
            gmwin.pos.y = y*resolution;
            gmwin.pos.h = resolution;
            gmwin.pos.w = resolution;
            SDL_RenderFillRect(gmwin.renderer, &gmwin.pos);
        }
    }
    for(int i = 0; i < threads.size(); i++){
        threads[i] = nullptr;
    }
    threads.clear();
}

void RenderMandelTrace(win &gmwin){
    Vector3 camera_right = cameraVector.cross(cameraUpVector);
    Vector3 camera_direction = cameraVector*zoom;
    for(int ix = 0; ix < xrays; ix++){
        for(int iy = 0; iy < yrays; iy++){
            double width = xrays;  // pixels across
            double height = yrays;  // pixels high
            double normalized_i = (ix / width) - xrays/width/2;
            double normalized_j = (iy / height) - yrays/height/2;

            double imagepointx = (camera_right * normalized_i).i + (cameraUpVector * normalized_j).i + camera_direction.x + camera_direction.i;
            double imagepointy = (camera_right * normalized_i).j + (cameraUpVector * normalized_j).j + camera_direction.y + camera_direction.j;
            double imagepointz = (camera_right * normalized_i).k + (cameraUpVector * normalized_j).k + camera_direction.z + camera_direction.k;
            Vector3 rayVector = Vector3(imagepointx - camera_direction.x, imagepointy - camera_direction.y, imagepointz - camera_direction.z, camera_direction.x, camera_direction.y, camera_direction.z);
            
            minDistances[ix][yrays-iy-1] = mandelSphereTracing(rayVector);
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

void sphereTracingThread(Vector3 camera_direction, Vector3 camera_right, int i){
    for(int ix = i*4; ix < i*4+4; ix++){
        for(int iy = 0; iy < yrays; iy++){
            double width = xrays;  // pixels across
            double height = yrays;  // pixels high
            double normalized_i = (ix / width) - xrays/width/2;
            double normalized_j = (iy / height) - yrays/height/2;
            
            double imagepointx = (camera_right * normalized_i).i + (cameraUpVector * normalized_j).i + camera_direction.x + camera_direction.i;
            double imagepointy = (camera_right * normalized_i).j + (cameraUpVector * normalized_j).j + camera_direction.y + camera_direction.j;
            double imagepointz = (camera_right * normalized_i).k + (cameraUpVector * normalized_j).k + camera_direction.z + camera_direction.k;
            Vector3 rayVector = Vector3(imagepointx - camera_direction.x, imagepointy - camera_direction.y, imagepointz - camera_direction.z, camera_direction.x, camera_direction.y, camera_direction.z);
            
            pair<double, Shape*> minDist = calcMinDist(rayVector.x, rayVector.y, rayVector.z);
            vector<double> angs = rayVector.getAngles();
            double stepox = rayVector.x;
            double stepoy = rayVector.y;
            double stepoz = rayVector.z;
            double pstepox = rayVector.x;
            double pstepoy = rayVector.y;
            double psteooz = rayVector.z;
            double iters = 0;

            while(minDist.first > .1 && minDist.first < 2000){
                iters++;
                stepox = stepox + minDist.first*cos(angs[0]);
                stepoy = stepoy + minDist.first*cos(angs[1]);
                stepoz = stepoz + minDist.first*cos(angs[2]);
                minDist = calcMinDist(stepox, stepoy, stepoz);
            }

            if(minDist.first <= .1){
                minDistances[ix][yrays-iy-1] = {sqrt(pow(stepox-rayVector.x, 2) + pow(stepoy-rayVector.y, 2) + pow(stepoz-rayVector.z, 2)), pointInShadow(stepox, stepoy, stepoz), static_cast<double>(minDist.second->color.r), static_cast<double>(minDist.second->color.g), static_cast<double>(minDist.second->color.b), iters};
            }
            else{
                minDistances[ix][yrays-iy-1] = {10000, 1, 0, 0, 0, iters}; //distance shadow r g b iters
            }
        }
    }
}

vector<double> sphereTracing(Vector3 &rayVector){
    pair<double, Shape*> minDist = calcMinDist(cameraVector.x, cameraVector.y, cameraVector.z);
    vector<double> angs = rayVector.getAngles();
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
}

void mandelSphereTracingThread(Vector3 camera_direction, Vector3 camera_right, int i){
    for(int ix = i*4; ix < i*4+4; ix++){
        for(int iy = 0; iy < yrays; iy++){
            double width = xrays;  // pixels across
            double height = yrays;  // pixels high
            double normalized_i = (ix / width) - xrays/width/2;
            double normalized_j = (iy / height) - yrays/height/2;

            double imagepointx = (camera_right * normalized_i).i + (cameraUpVector * normalized_j).i + camera_direction.x + camera_direction.i;
            double imagepointy = (camera_right * normalized_i).j + (cameraUpVector * normalized_j).j + camera_direction.y + camera_direction.j;
            double imagepointz = (camera_right * normalized_i).k + (cameraUpVector * normalized_j).k + camera_direction.z + camera_direction.k;
            Vector3 rayVector = Vector3(imagepointx - camera_direction.x, imagepointy - camera_direction.y, imagepointz - camera_direction.z, camera_direction.x, camera_direction.y, camera_direction.z);
            
            double minDist = distMandelBulb(rayVector.x, rayVector.y, rayVector.z);
            vector<double> angs = rayVector.getAngles();
            double stepox = rayVector.x;
            double stepoy = rayVector.y;
            double stepoz = rayVector.z;
            double pstepox = rayVector.x;
            double pstepoy = rayVector.y;
            double psteooz = rayVector.z;
            double iters = 0;

            while(minDist > .01 && minDist < 2000){
                iters++;
                stepox = stepox + minDist*cos(angs[0]);
                stepoy = stepoy + minDist*cos(angs[1]);
                stepoz = stepoz + minDist*cos(angs[2]);
                minDist = distMandelBulb(stepox, stepoy, stepoz);
            }

            if(minDist <= .01){
                minDistances[ix][yrays-iy-1] = {sqrt(pow(stepox-cameraVector.x, 2) + pow(stepoy-cameraVector.y, 2) + pow(stepoz-cameraVector.z, 2)), iters};
            }
            else{
                minDistances[ix][yrays-iy-1] = {10000, iters};
            }
        }
    }
}

vector<double> mandelSphereTracing(Vector3 &rayVector){
    double minDist = distMandelBulb(cameraVector.x, cameraVector.y, cameraVector.z);
    vector<double> angs = rayVector.getAngles();
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

double smoothMin(double a, double b, double k){
    double h = clamp((b - a + k) / (2 * k), 0, 1);
    return a * h + b  * (1 - h) - k  * h  * (1 - h);
    // double h = clamp(.5 + .5 *(b-a)/k, 0.0, 1.0);
    // return mix(b, a, h) - k*h*(1.0-h);
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

void keyCases(std::chrono::duration<double> elapsed_seconds){
    if(l){
        Vector3 camera_right = cameraVector.cross(cameraUpVector);
        Vector3 camera_direction = cameraVector*turnStep;

        double width = xrays;  // pixels across
        double height = yrays;  // pixels high
        double normalized_i = 0 - .5;
        double normalized_j = .5 - .5;

        double imagepointx = (camera_right * normalized_i).i + (cameraUpVector * normalized_j).i + camera_direction.x + camera_direction.i;
        double imagepointy = (camera_right * normalized_i).j + (cameraUpVector * normalized_j).j + camera_direction.y + camera_direction.j;
        double imagepointz = (camera_right * normalized_i).k + (cameraUpVector * normalized_j).k + camera_direction.z + camera_direction.k;
        Vector3 rayVector = Vector3(imagepointx - camera_direction.x, imagepointy - camera_direction.y, imagepointz - camera_direction.z, camera_direction.x, camera_direction.y, camera_direction.z);
        rayVector.normalize();
        cameraVector = rayVector;
    }
    if(r){
        Vector3 camera_right = cameraVector.cross(cameraUpVector);
        Vector3 camera_direction = cameraVector*turnStep;

        double width = xrays;  // pixels across
        double height = yrays;  // pixels high
        double normalized_i = 1 - .5;
        double normalized_j = .5 - .5;

        double imagepointx = (camera_right * normalized_i).i + (cameraUpVector * normalized_j).i + camera_direction.x + camera_direction.i;
        double imagepointy = (camera_right * normalized_i).j + (cameraUpVector * normalized_j).j + camera_direction.y + camera_direction.j;
        double imagepointz = (camera_right * normalized_i).k + (cameraUpVector * normalized_j).k + camera_direction.z + camera_direction.k;
        Vector3 rayVector = Vector3(imagepointx - camera_direction.x, imagepointy - camera_direction.y, imagepointz - camera_direction.z, camera_direction.x, camera_direction.y, camera_direction.z);
        rayVector.normalize();
        cameraVector = rayVector;
    }
    if(up){
        Vector3 camera_right = cameraVector.cross(cameraUpVector);
        Vector3 camera_direction = cameraVector*turnStep;

        double width = xrays;  // pixels across
        double height = yrays;  // pixels high
        double normalized_i = .5 - .5;
        double normalized_j = 1 - .5;

        double imagepointx = (camera_right * normalized_i).i + (cameraUpVector * normalized_j).i + camera_direction.x + camera_direction.i;
        double imagepointy = (camera_right * normalized_i).j + (cameraUpVector * normalized_j).j + camera_direction.y + camera_direction.j;
        double imagepointz = (camera_right * normalized_i).k + (cameraUpVector * normalized_j).k + camera_direction.z + camera_direction.k;
        Vector3 rayVector = Vector3(imagepointx - camera_direction.x, imagepointy - camera_direction.y, imagepointz - camera_direction.z, camera_direction.x, camera_direction.y, camera_direction.z);
        rayVector.normalize();
        cameraVector = rayVector;
        cameraUpVector = camera_right.cross(cameraVector);
    }
    if(down){
        Vector3 camera_right = cameraVector.cross(cameraUpVector);
        Vector3 camera_direction = cameraVector*turnStep;

        double width = xrays;  // pixels across
        double height = yrays;  // pixels high
        double normalized_i = .5 - .5;
        double normalized_j = 0 - .5;

        double imagepointx = (camera_right * normalized_i).i + (cameraUpVector * normalized_j).i + camera_direction.x + camera_direction.i;
        double imagepointy = (camera_right * normalized_i).j + (cameraUpVector * normalized_j).j + camera_direction.y + camera_direction.j;
        double imagepointz = (camera_right * normalized_i).k + (cameraUpVector * normalized_j).k + camera_direction.z + camera_direction.k;
        Vector3 rayVector = Vector3(imagepointx - camera_direction.x, imagepointy - camera_direction.y, imagepointz - camera_direction.z, camera_direction.x, camera_direction.y, camera_direction.z);
        rayVector.normalize();
        cameraVector = rayVector;
        cameraUpVector = camera_right.cross(cameraVector);
    }
    if(e){
        Vector3 camera_right = cameraUpVector.cross(cameraVector);
        Vector3 camera_direction = cameraUpVector*5;
        Vector3 camera_up_direction = cameraVector;

        double width = xrays;  // pixels across
        double height = yrays;  // pixels high
        double normalized_i = 0 - .5;
        double normalized_j = .5 - .5;

        double imagepointx = (camera_right * normalized_i).i + (camera_up_direction * normalized_j).i + camera_direction.x + camera_direction.i;
        double imagepointy = (camera_right * normalized_i).j + (camera_up_direction * normalized_j).j + camera_direction.y + camera_direction.j;
        double imagepointz = (camera_right * normalized_i).k + (camera_up_direction * normalized_j).k + camera_direction.z + camera_direction.k;
        Vector3 rayVector = Vector3(imagepointx - camera_direction.x, imagepointy - camera_direction.y, imagepointz - camera_direction.z, camera_direction.x, camera_direction.y, camera_direction.z);
        rayVector.normalize();
        cameraUpVector = rayVector;
    }
    if(q){
        Vector3 camera_right = cameraUpVector.cross(cameraVector);
        Vector3 camera_direction = cameraUpVector*5;
        Vector3 camera_up_direction = cameraVector;

        double width = xrays;  // pixels across
        double height = yrays;  // pixels high
        double normalized_i = 1 - .5;
        double normalized_j = .5 - .5;

        double imagepointx = (camera_right * normalized_i).i + (camera_up_direction * normalized_j).i + camera_direction.x + camera_direction.i;
        double imagepointy = (camera_right * normalized_i).j + (camera_up_direction * normalized_j).j + camera_direction.y + camera_direction.j;
        double imagepointz = (camera_right * normalized_i).k + (camera_up_direction * normalized_j).k + camera_direction.z + camera_direction.k;
        Vector3 rayVector = Vector3(imagepointx - camera_direction.x, imagepointy - camera_direction.y, imagepointz - camera_direction.z, camera_direction.x, camera_direction.y, camera_direction.z);
        rayVector.normalize();
        cameraUpVector = rayVector;
    }
    if(w){
        if(m == 0){
            shapes[m]->centery += step*elapsed_seconds.count();
        }
        if(m == 1){
            shapes[m]->centery += step*elapsed_seconds.count();
        }
        else if(m == 3){
            shapes[m]->centery += step*elapsed_seconds.count();
        }
        else if(m == 4){
            pox = cameraVector.x;
            poy = cameraVector.y;
            poz = cameraVector.z;
            cameraVector.x += step*elapsed_seconds.count()*cameraVector.i;
            cameraVector.y += step*elapsed_seconds.count()*cameraVector.j;
            cameraVector.z += step*elapsed_seconds.count()*cameraVector.k;
            if(calcMinDist(cameraVector.x, cameraVector.y, cameraVector.z).first < .01){
                cameraVector.x = pox;
                cameraVector.y = poy;
                cameraVector.z = poz;
            }
        }
        else if(m == 5){
            pox = cameraVector.x;
            poy = cameraVector.y;
            poz = cameraVector.z;
            cameraVector.x += step*elapsed_seconds.count()*cameraVector.i;
            cameraVector.y += step*elapsed_seconds.count()*cameraVector.j;
            cameraVector.z += step*elapsed_seconds.count()*cameraVector.k;
            if(distMandelBulb(cameraVector.x, cameraVector.y, cameraVector.z) < .01){
                cameraVector.x = pox;
                cameraVector.y = poy;
                cameraVector.z = poz;
            }
        }
    }
    if(s){
        if(m == 0){
            shapes[m]->centery -= step*elapsed_seconds.count();
        }
        else if(m == 1){
            shapes[m]->centery -= step*elapsed_seconds.count();
        }
        else if(m == 2){
            shapes[m]->centery -= step*elapsed_seconds.count();
        }
        else if(m == 3){
            pox = cameraVector.x;
            poy = cameraVector.y;
            poz = cameraVector.z;
            cameraVector.x -= step*elapsed_seconds.count()*cameraVector.i;
            cameraVector.y -= step*elapsed_seconds.count()*cameraVector.j;
            cameraVector.z -= step*elapsed_seconds.count()*cameraVector.k;
            if(calcMinDist(cameraVector.x, cameraVector.y, cameraVector.z).first < .01){
                cameraVector.x = pox;
                cameraVector.y = poy;
                cameraVector.z = poz;
            }
        }
        else if(m == 4){
            pox = cameraVector.x;
            poy = cameraVector.y;
            poz = cameraVector.z;
            cameraVector.x -= step*elapsed_seconds.count()*cameraVector.i;
            cameraVector.y -= step*elapsed_seconds.count()*cameraVector.j;
            cameraVector.z -= step*elapsed_seconds.count()*cameraVector.k;
            if(distMandelBulb(cameraVector.x, cameraVector.y, cameraVector.z) < .01){
                cameraVector.x = pox;
                cameraVector.y = poy;
                cameraVector.z = poz;
            }
        }
    }
    if(a){
        if(m == 0){
            shapes[m]->centerx -= step*elapsed_seconds.count();
        }
        else if(m == 1){
            shapes[m]->centerx -= step*elapsed_seconds.count();
        }
        else if(m == 2){
            shapes[m]->centerx -= step*elapsed_seconds.count();
        }
        else if(m == 3){
            Vector3 camera_right = cameraVector.cross(cameraUpVector);
            pox = cameraVector.x;
            poy = cameraVector.y;
            poz = cameraVector.z;
            cameraVector.x -= step*elapsed_seconds.count()*camera_right.i;
            cameraVector.y -= step*elapsed_seconds.count()*camera_right.j;
            cameraVector.z -= step*elapsed_seconds.count()*camera_right.k;
            if(calcMinDist(cameraVector.x, cameraVector.y, cameraVector.z).first < .01){
                cameraVector.x = pox;
                cameraVector.y = poy;
                cameraVector.z = poz;
            }
        }
        else if(m == 4){
            Vector3 camera_right = cameraVector.cross(cameraUpVector);
            pox = cameraVector.x;
            poy = cameraVector.y;
            poz = cameraVector.z;
            cameraVector.x -= step*elapsed_seconds.count()*camera_right.i;
            cameraVector.y -= step*elapsed_seconds.count()*camera_right.j;
            cameraVector.z -= step*elapsed_seconds.count()*camera_right.k;
            if(distMandelBulb(cameraVector.x, cameraVector.y, cameraVector.z) < .01){
                cameraVector.x = pox;
                cameraVector.y = poy;
                cameraVector.z = poz;
            }
        }
    }
    if(d){
        if(m == 0){
            shapes[m]->centerx += step*elapsed_seconds.count();
        }
        else if(m == 1){
            shapes[m]->centerx += step*elapsed_seconds.count();
        }
        else if(m == 2){
            shapes[m]->centerx += step*elapsed_seconds.count();
        }
        else if(m == 3){
            Vector3 camera_right = cameraVector.cross(cameraUpVector);
            pox = cameraVector.x;
            poy = cameraVector.y;
            poz = cameraVector.z;
            cameraVector.x += step*elapsed_seconds.count()*camera_right.i;
            cameraVector.y += step*elapsed_seconds.count()*camera_right.j;
            cameraVector.z += step*elapsed_seconds.count()*camera_right.k;
            if(calcMinDist(cameraVector.x, cameraVector.y, cameraVector.z).first < .01){
                cameraVector.x = pox;
                cameraVector.y = poy;
                cameraVector.z = poz;
            }
        }
        else if(m == 4){
            Vector3 camera_right = cameraVector.cross(cameraUpVector);
            pox = cameraVector.x;
            poy = cameraVector.y;
            poz = cameraVector.z;
            cameraVector.x += step*elapsed_seconds.count()*camera_right.i;
            cameraVector.y += step*elapsed_seconds.count()*camera_right.j;
            cameraVector.z += step*elapsed_seconds.count()*camera_right.k;
            if(distMandelBulb(cameraVector.x, cameraVector.y, cameraVector.z) < .01){
                cameraVector.x = pox;
                cameraVector.y = poy;
                cameraVector.z = poz;
            }
        }
    }
}