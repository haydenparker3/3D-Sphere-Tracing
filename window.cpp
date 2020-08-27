#include "common.h"
#include "cmath"
#include "string"
#include "window.h"
#include "algorithm"

win :: win(){
    screenWidth = 600;
    screenHeight = 600;
    //Error Checking/Initialisation
    if (!Init())
    {
        printf("Failed to Initialize");
        throw -1;
    }

    // Clear buffer with black background
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    //Swap Render Buffers
    SDL_GL_SwapWindow(window);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
}

win :: win(int width, int height){
    screenWidth = width;
    screenHeight = height;
    //Error Checking/Initialisation
    if (!Init())
    {
        printf("Failed to Initialize");
        throw -1;
    }

    // Clear buffer with black background
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    //Swap Render Buffers
    SDL_GL_SwapWindow(window);
    //SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
    SDL_GetWindowSize(window, &screenWidth, &screenHeight);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
}

win :: ~win(){
    CleanUp();
}

bool win :: Init()
{
    if (SDL_Init(SDL_INIT_NOPARACHUTE & SDL_INIT_EVERYTHING) != 0)
    {
        SDL_Log("Unable to initialize SDL: %s\n", SDL_GetError());
        return false;
    }
    else
    {
        //Specify OpenGL Version (4.2)
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_Log("SDL Initialised");
    }

    //Create Window Instance
    window = SDL_CreateWindow(
        "Game Engine",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        screenWidth,
        screenHeight,   
        SDL_WINDOW_OPENGL);

    //Check that the window was succesfully created
    if (window == NULL)
    {
        //Print error, if null
        printf("Could not create window: %s\n", SDL_GetError());
        return false;
    }
    else{
        gScreenSurface = SDL_GetWindowSurface(window);
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        SDL_Log("Window Successful Generated");
    }
    //Map OpenGL Context to Window
    glContext = SDL_GL_CreateContext(window);

    return true;
}

void win :: CleanUp()
{
    //Free up resources
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void win :: ClearWindow(){

    pos.x = 0;
    pos.y = 0;
    pos.w = screenWidth;
    pos.h = screenHeight;
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
    SDL_RenderFillRect(renderer, &pos);
}

SDL_Texture* win :: CreateTexture(string imgsrc){
    return IMG_LoadTexture(renderer, imgsrc.c_str());
}

void win :: RenderImage(SDL_Texture *img, int posx, int posy, int width, int height){
    int w, h; //image width and height
    SDL_QueryTexture(img, NULL, NULL, &w, &h);
    pos.x = posx; pos.y = posy; pos.w = width; pos.h = height;
    SDL_RenderCopy(renderer, img, NULL, &pos);
    img = nullptr;
}

void win :: DrawCircle(bool fill, int centerx, int centery, int radius, SDL_Color color, int offx, int offy)
{
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    double x = centerx - radius;
    double y = centery;
    double endx = centerx + radius;

    if(x < 0)
        x = 0;
    if(endx >= screenWidth)
        endx = screenWidth - 1;

    if(fill){
        for(x; x <= endx; x+=1){
            double h = sqrt(radius*radius - (x - centerx) * (x - centerx));
            
            double uh = centery+h;
            if(uh > screenHeight)
                uh = screenHeight;
            
            double dh = centery-h;
            if(dh < 0)
                dh = 0;

            SDL_RenderDrawLine(renderer, x+offx, uh+offy, x+offx, dh+offy);
        }
    }
    else{
        int dprevy, uprevy;
        int ox = x;
        for(x; x <= endx; x+=1){
            double h = sqrt(radius*radius - (x - centerx) * (x - centerx));
            
            double uh = centery+h;
            if(uh > screenHeight)
                uh = screenHeight;
            
            double dh = centery-h;
            if(dh < 0)
                dh = 0;
            
            
            // SDL_RenderDrawPoint(renderer, x, uh);
            // SDL_RenderDrawPoint(renderer, x, dh);
            if(x-1 >= ox){
                if(sqrt(1 + pow(2, dprevy - dh)) > 1){
                    SDL_RenderDrawLine(renderer, x-1+offx, dprevy+offy, x+offx, dh+offy);
                    SDL_RenderDrawLine(renderer, x-1+offx, uprevy+offy, x+offx, uh+offy);
                }
            }
            dprevy = dh;
            uprevy = uh;
        }
    }
}

bool win :: ScreenIntersect(int centerx, int centery, int radius){
    bool seg1 = SegmentIntersectCircle(radius, 0-centerx, 0-centery, screenWidth-centerx, 0-centery);
    bool seg2 = SegmentIntersectCircle(radius, 0-centerx, 0-centery, 0-centerx, screenHeight-centery);
    bool seg3 = SegmentIntersectCircle(radius, screenWidth-centerx, 0-centery, screenWidth-centerx, screenHeight-centery);
    bool seg4 = SegmentIntersectCircle(radius, 0-centerx, screenHeight-centery, screenWidth-centerx, screenHeight-centery);
    if(seg1 || seg2 || seg3 || seg4){
        return true;
    }
    if(centerx > 0 && centerx < screenWidth && centery > 0 && centery < screenHeight){
        if(radius < min(screenHeight/2, screenWidth/2)){
            return true;
        }
    }
    
    return false;
}

bool win :: SegmentIntersectCircle(int radius, int x1, int y1, int x2, int y2){
    double dx = x2 - x1; 
    double dy = y2 - y1; 
    double dr = sqrt(static_cast<double>(pow(dx, 2) + pow(dy, 2)));
    double D = x1*y2 - x2*y1;

    double check = radius*radius*dr*dr - D*D; 
    double ix1 = D*dy + ((dy < 0) ? -1 : 1)*dx*sqrt(radius*radius*dr*dr - D*D);
    ix1 = ix1 / (dr*dr);
    double iy1 = -1*D*dx + abs(dy)*sqrt(radius*radius*dr*dr - D*D);
    iy1 = iy1 / (dr*dr);

    double ix2 = D*dy - ((dy < 0) ? -1 : 1)*dx*sqrt(radius*radius*dr*dr - D*D);
    ix2 = ix2 / (dr*dr);
    double iy2 = -1*D*dx - abs(dy)*sqrt(radius*radius*dr*dr - D*D);
    iy2 = iy2 / (dr*dr);

    if(check > 0){
        if(ix1 <= x2 && ix1 >= x1 && iy1 <= y2 && iy1 >= y1){
            return true;
        }
        else if(ix2 <= x2 && ix2 >= x1 && iy2 <= y2 && iy2 >= y1){
            return true;
        }
    }
    return false;
}

void win :: DrawRectangle(bool fill, int centerx, int centery, int width, int height, SDL_Color color, int offx, int offy){
    double x = centerx - width/2;
    double y = centery - height/2;
    double endx = x + width;

    if(x < 0)
        x = 0;
    if(endx > screenWidth)
        endx = screenWidth-1;

    if(fill){
        for(x; x <= endx; x+=1){
            double h = height;
            
            if(y + h > screenHeight)
                h =  screenHeight - 1;

            SDL_RenderDrawLine(renderer, x+offx, y+offy, x+offx, y+h+offy);
        }
    }
    else{
        
    }
}