#include "game.h"
#include <stdio.h>

int main(void){
    if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_EVENTS)!=0){
        fprintf(stderr,"Erro ao iniciar SDL2: %s\n",SDL_GetError());return 1;
    }
    SDL_Window*win=SDL_CreateWindow("Letras Invasoras - Mao Esquerda",
        SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,SCREEN_W,SCREEN_H,
        SDL_WINDOW_SHOWN|SDL_WINDOW_RESIZABLE);
    if(!win){fprintf(stderr,"Janela: %s\n",SDL_GetError());SDL_Quit();return 1;}
    SDL_Renderer*r=SDL_CreateRenderer(win,-1,SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);
    if(!r)r=SDL_CreateRenderer(win,-1,SDL_RENDERER_SOFTWARE);
    if(!r){fprintf(stderr,"Renderer: %s\n",SDL_GetError());SDL_DestroyWindow(win);SDL_Quit();return 1;}
    SDL_RenderSetLogicalSize(r,SCREEN_W,SCREEN_H);
    SDL_SetRenderDrawBlendMode(r,SDL_BLENDMODE_BLEND);
    if(!audio_init())fprintf(stderr,"Audio indisponivel: %s\n",SDL_GetError());
    Game game;game_init(&game);
    bool running=true;
    Uint64 prev=SDL_GetPerformanceCounter();
    while(running){
        SDL_Event event;
        while(SDL_PollEvent(&event)){
            if(event.type==SDL_QUIT)running=false;
            else if(event.type==SDL_KEYDOWN&&!event.key.repeat){
                if(event.key.keysym.scancode==SDL_SCANCODE_ESCAPE)running=false;
                else game_key(&game,event.key.keysym.scancode);
            }
        }
        Uint64 now=SDL_GetPerformanceCounter();
        float dt=(float)((double)(now-prev)/SDL_GetPerformanceFrequency());prev=now;
        audio_pause(game.paused||game.finished);
        game_update(&game,dt);render_game(r,&game);
        SDL_Delay(5);
    }
    audio_shutdown();
    SDL_DestroyRenderer(r);SDL_DestroyWindow(win);SDL_Quit();return 0;
}
