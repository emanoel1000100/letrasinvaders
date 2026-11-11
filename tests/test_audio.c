/* Offline synthesis + real SDL dummy-device/game integration. */
#include "../src/audio.c"
#include "game.h"
#include <assert.h>
#include <stdio.h>

static double energy(const float *samples,int n){
    double sum=0;for(int i=0;i<n;i++){assert(isfinite(samples[i]));assert(fabsf(samples[i])<1);sum+=samples[i]*samples[i];}
    return sum/n;
}
int main(void){
    float music[4800],shot[4800],quiet[4800];
    AudioState bg={0},fx={0};
    audio_mix(&bg,music,4800);
    fx.shot_left=SHOT_SAMPLES;
    audio_mix(&fx,shot,4800);
    assert(energy(music,4800)>0.000001);
    assert(energy(shot,4800)>energy(music,4800)*2);
    bg.paused=true; audio_mix(&bg,quiet,4800);
    assert(energy(quiet,4800)==0);
    bg.paused=false; audio_mix(&bg,music,4800);
    assert(energy(music,4800)>0);
    assert(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER)==0);
    assert(audio_init());
    SDL_LockAudioDevice(device);
    Game game;game_init(&game);
    assert(state.shot_left==0);
    game_key(&game,(SDL_Scancode)(SDL_SCANCODE_A+required_key(&game)-'A'));
    assert(game.locked && !game.firing);
    SDL_UnlockAudioDevice(device);
    game_key(&game,SDL_SCANCODE_F);
    SDL_LockAudioDevice(device);
    assert(game.firing && state.shot_left>0);
    SDL_UnlockAudioDevice(device);
    audio_pause(true); assert(state.paused);
    audio_pause(false); assert(!state.paused);
    SDL_Surface *surface=SDL_CreateRGBSurfaceWithFormat(0,SCREEN_W,SCREEN_H,32,SDL_PIXELFORMAT_RGBA32);
    assert(surface); SDL_Renderer *renderer=SDL_CreateSoftwareRenderer(surface);assert(renderer);
    render_game(renderer,&game);
    for(int i=0;i<200;i++)game_update(&game,0.016f);
    assert(game.total==1);
    render_game(renderer,&game);
    SDL_DestroyRenderer(renderer);SDL_FreeSurface(surface);
    audio_shutdown();audio_shutdown();SDL_Quit();
    printf("PASS: synthesis, shot louder than music, silence/resume, actual shot trigger, render, hit, shutdown\n");
    return 0;
}
