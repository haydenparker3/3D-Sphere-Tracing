#pragma once
#include "common.h"
#include "cmath"
#include "string"

class win{
    public:
        SDL_Window *window;
        SDL_GLContext glContext;
        SDL_Surface *gScreenSurface = nullptr;
        SDL_Renderer *renderer = nullptr;
        SDL_Rect pos;

        int screenWidth;
        int screenHeight;

        win();
        win(int width, int height);
        ~win();
        bool Init();
        void CleanUp();
        void ClearWindow();
        void RenderImage(SDL_Texture *img, int posx, int posy, int width, int height);
        SDL_Texture* CreateTexture(string imgsrc);
        void DrawCircle(bool fill, int centerx, int centery, int radius, SDL_Color color, int offx, int offy);
        bool ScreenIntersect(int centerx, int centery, int radius);
        bool SegmentIntersectCircle(int radius, int px1, int py1, int px2, int py2);
        void DrawRectangle(bool fill, int centerx, int centery, int width, int height, SDL_Color color, int offx, int offy);
};