#include "game.h"
#include <math.h>
SDL_Color rgb(int r,int g,int b){SDL_Color c={(Uint8)r,(Uint8)g,(Uint8)b,255};return c;}
static void color(SDL_Renderer*r,SDL_Color c){SDL_SetRenderDrawColor(r,c.r,c.g,c.b,c.a);}
void fill_rect(SDL_Renderer*r,int x,int y,int w,int h,SDL_Color c){SDL_Rect q={x,y,w,h};color(r,c);SDL_RenderFillRect(r,&q);} 
void stroke_rect(SDL_Renderer*r,int x,int y,int w,int h,SDL_Color c){SDL_Rect q={x,y,w,h};color(r,c);SDL_RenderDrawRect(r,&q);} 
void draw_line(SDL_Renderer*r,int x1,int y1,int x2,int y2,SDL_Color c){color(r,c);SDL_RenderDrawLine(r,x1,y1,x2,y2);} 
void fill_circle(SDL_Renderer*r,int cx,int cy,int radius,SDL_Color c){
    color(r,c);
    for(int dy=-radius;dy<=radius;dy++){
        int dx=(int)sqrt((double)(radius*radius-dy*dy));
        SDL_RenderDrawLine(r,cx-dx,cy+dy,cx+dx,cy+dy);
    }
}
void stroke_circle(SDL_Renderer*r,int cx,int cy,int radius,SDL_Color c){
    color(r,c);
    const int steps=48;
    for(int i=0;i<steps;i++){
        float a0=(float)i/steps*6.2831853f;
        float a1=(float)(i+1)/steps*6.2831853f;
        SDL_RenderDrawLine(r,
            cx+(int)(cosf(a0)*radius),cy+(int)(sinf(a0)*radius),
            cx+(int)(cosf(a1)*radius),cy+(int)(sinf(a1)*radius));
    }
}
