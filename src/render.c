#include "game.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#define PI 3.14159265358979323846f

static void background(SDL_Renderer*r,const Game*g){
    fill_rect(r,0,0,SCREEN_W,SCREEN_H,rgb(7,12,28));
    for(int i=0;i<90;i++){
        int x=(i*137+91)%SCREEN_W;
        int y=(int)(i*233+g->time*(5+i%4*6))%643;
        int z=70+i%100;
        fill_rect(r,x,y,(i%11==0)?2:1,(i%11==0)?2:1,rgb(z,z+10,z+42));
    }
    fill_rect(r,0,0,SCREEN_W,110,rgb(20,30,58));
    fill_rect(r,0,110,SCREEN_W,3,rgb(44,74,135));
    draw_center(r,480,14,"LETRAS INVASORAS",4,rgb(115,240,205));
    draw_center(r,480,54,g->level==3?"DIGITE A PALAVRA E ATIRE COM F":"DIGITE A LETRA E ATIRE COM F",2,rgb(244,237,199));
    char buf[120];
    snprintf(buf,sizeof buf,"NIVEL %d/3   ACERTOS %d/%d   ERROS %d",g->level,g->hits,level_goal(g->level),g->errors);
    draw_center(r,480,84,buf,2,rgb(180,206,241));
    fill_rect(r,12,106,936,4,rgb(40,53,82));
    fill_rect(r,12,106,936*g->hits/level_goal(g->level),4,rgb(106,229,191));
}
static void draw_word_progress(SDL_Renderer*r,const Game*g){
    int len=(int)strlen(g->target),gap=22,scale=(len>7?2:3);
    int total=len*(5*scale)+(len-1)*gap;
    int x=480-total/2;
    for(int i=0;i<len;i++){
        char s[2]={g->target[i],0};
        SDL_Color c=i<g->typed?rgb(112,239,178):(i==g->typed?rgb(255,224,146):rgb(184,197,224));
        draw_text(r,x,122,s,scale,c);
        fill_rect(r,x-3,149,5*scale+6,4,i<g->typed?rgb(112,239,178):rgb(71,87,125));
        x+=5*scale+gap;
    }
}
static void draw_meteor(SDL_Renderer*r,int x,int y,char ch,bool focus,bool highlight){
    SDL_Color body=focus?rgb(126,97,72):rgb(107,85,66);
    SDL_Color edge=focus?rgb(255,218,137):rgb(176,144,109);
    SDL_Color crater1=rgb(77,58,44), crater2=rgb(143,113,84);
    if(highlight){
        stroke_circle(r,x,y,37,rgb(79,214,255));
        stroke_circle(r,x,y,39,rgb(79,214,255));
    }
    fill_circle(r,x,y,31,body);
    fill_circle(r,x-12,y-9,10,crater1);
    fill_circle(r,x+11,y+6,8,crater1);
    fill_circle(r,x+2,y-15,6,crater2);
    fill_circle(r,x-8,y+13,5,crater2);
    stroke_circle(r,x,y,31,edge);
    stroke_circle(r,x+5,y-3,28,rgb(84,66,51));
    char s[2]={ch,0};
    draw_center(r,x,y-12,s,4,focus?rgb(255,246,214):rgb(242,234,214));
}
static void meteor_swarm(SDL_Renderer*r,const Game*g){
    for(int i=0;i<g->meteor_count;i++){
        bool focus=(i==g->selected);
        bool highlight=(focus && (g->locked || (g->easy && !g->firing)));
        draw_meteor(r,(int)meteor_x(g,i),(int)meteor_y(g,i),g->meteor_letters[i],focus,highlight);
    }
}
/* Silhueta de uma nave de combate: triangulos coloridos e geometria SDL2.
   Desenhada em coordenadas locais para a rotacao apontar de verdade. */
static SDL_FPoint ship_point(float angle,float x,float y){
    float c=cosf(angle),s=sinf(angle);
    return (SDL_FPoint){480.0f+x*c-y*s,SHIP_Y+x*s+y*c};
}
static void ship_triangle(SDL_Renderer*r,float angle,
                          float ax,float ay,float bx,float by,float cx,float cy,SDL_Color color){
    SDL_Vertex v[3];
    v[0]=(SDL_Vertex){ship_point(angle,ax,ay),color,{0,0}};
    v[1]=(SDL_Vertex){ship_point(angle,bx,by),color,{0,0}};
    v[2]=(SDL_Vertex){ship_point(angle,cx,cy),color,{0,0}};
    SDL_RenderGeometry(r,NULL,v,3,NULL,0);
}
static void ship_line(SDL_Renderer*r,float a,int x1,int y1,int x2,int y2,SDL_Color col){
    SDL_FPoint p=ship_point(a,x1,y1),q=ship_point(a,x2,y2);
    draw_line(r,(int)p.x,(int)p.y,(int)q.x,(int)q.y,col);
}
static void ship(SDL_Renderer*r,const Game*g){
    float a=g->ship_angle;
    /* Asas em delta e cauda: silhueta espacial, nao apenas linhas. */
    ship_triangle(r,a,-8,-10,-42,23,-18,32,rgb(48,93,169));
    ship_triangle(r,a,8,-10,42,23,18,32,rgb(48,93,169));
    ship_triangle(r,a,-9,-16,-26,23,-8,18,rgb(107,168,230));
    ship_triangle(r,a,9,-16,26,23,8,18,rgb(107,168,230));
    ship_triangle(r,a,0,-46,-15,18,15,18,rgb(179,217,237));
    ship_triangle(r,a,0,-46,0,18,15,18,rgb(222,240,247));
    ship_triangle(r,a,-15,18,15,18,0,33,rgb(94,153,198));
    /* Cabine azul escuro, com brilho central. */
    ship_triangle(r,a,0,-25,-8,-3,8,-3,rgb(23,49,103));
    ship_triangle(r,a,0,-21,-4,-5,4,-5,rgb(64,205,250));
    ship_line(r,a,-41,23,-22,10,rgb(129,213,245));
    ship_line(r,a,41,23,22,10,rgb(129,213,245));
    ship_line(r,a,-14,18,-14,30,rgb(255,233,145));
    ship_line(r,a,14,18,14,30,rgb(255,233,145));
    int flame=(int)(sinf(g->time*17.0f)*4.0f);
    ship_triangle(r,a,-19,30,-10,30,-14,45+flame,rgb(255,142,58));
    ship_triangle(r,a,10,30,19,30,14,45+flame,rgb(255,142,58));
    ship_triangle(r,a,-17,31,-12,31,-14,39+flame,rgb(255,238,146));
    ship_triangle(r,a,12,31,17,31,14,39+flame,rgb(255,238,146));
}
static void hud(SDL_Renderer*r,const Game*g){
    char s[120];
    if(g->level==3){
        draw_center(r,480,118,"PALAVRA:",2,rgb(252,229,168));
        draw_word_progress(r,g);
        snprintf(s,sizeof s,"PROXIMA TECLA: %c",required_key(g));
        draw_center(r,480,165,s,2,rgb(192,215,245));
    }else{
        snprintf(s,sizeof s,"LETRA: %c   DEDO: %s",required_key(g),finger_name(required_key(g)));
        draw_center(r,480,126,s,2,rgb(250,226,171));
    }
    if(g->locked||g->easy){
        float tx=target_x(g),ty=target_y(g);
        SDL_Color beam=g->locked?rgb(101,248,202):rgb(67,101,144);
        draw_line(r,480,(int)SHIP_Y-37,(int)tx,(int)ty,beam);
    }
    if(g->error_flash>0)draw_center(r,480,503,"ERROU - SIGA A TECLA ACESA",2,rgb(255,143,172));
    else if(g->locked)draw_center(r,480,503,"MIRA TRAVADA! APERTE F",2,rgb(114,252,198));
    else if(g->level==3)draw_center(r,480,503,"UMA LETRA + F PARA CADA METEORO",2,rgb(195,209,244));
    if(g->level_flash>0)draw_center(r,480,320,"NOVA FASE!",4,rgb(255,222,141));
    if(g->easy)draw_text(r,21,613,"MODO FACIL",2,rgb(151,228,194));
    draw_text(r,690,613,"F = BASE / TIRO",2,rgb(158,189,231));
}
static void overlays(SDL_Renderer*r,const Game*g){
    if(!g->paused&&!g->finished)return;
    fill_rect(r,160,205,640,250,(SDL_Color){8,22,45,245});
    stroke_rect(r,160,205,640,250,rgb(126,223,216));
    if(g->finished){
        draw_center(r,480,235,"PARABENS!",6,rgb(255,225,137));
        draw_center(r,480,324,"VOCE CONCLUIU AS 3 FASES",2,rgb(177,243,216));
        draw_center(r,480,377,"APERTE F1 PARA RECOMECAR",2,rgb(210,224,242));
    }else{
        draw_center(r,480,236,"PAUSA",6,rgb(255,225,137));
        draw_center(r,480,337,"APERTE ESPACO PARA VOLTAR",2,rgb(210,224,242));
    }
}
void render_game(SDL_Renderer*r,const Game*g){
    background(r,g);
    meteor_swarm(r,g);
    hud(r,g);
    ship(r,g);
    if(g->firing){
        fill_circle(r,(int)g->shot_x,(int)g->shot_y,5,rgb(255,244,163));
        fill_circle(r,(int)g->shot_x,(int)g->shot_y,2,rgb(255,255,255));
    }
    for(int i=0;i<PARTICLE_MAX;i++)if(g->particles[i].life>0){
        const Particle*p=&g->particles[i];
        fill_rect(r,(int)p->x,(int)p->y,5,5,p->color);
    }
    draw_keyboard(r,g);
    overlays(r,g);
    SDL_RenderPresent(r);
}
