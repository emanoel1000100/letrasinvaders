#include "game.h"
#include <math.h>
#include <string.h>

#define SHIP_X 480.0f
#define PI 3.14159265358979323846f

float meteor_x(const Game*g,int idx){return g->meteor_pos_x[idx];}
float meteor_y(const Game*g,int idx){return g->meteor_pos_y[idx]+g->swarm_y+sinf(g->time*0.9f+idx*0.7f)*4.0f;}
float target_x(const Game*g){return meteor_x(g,g->selected);}
float target_y(const Game*g){return meteor_y(g,g->selected);}
static float clampf(float x,float a,float b){return x<a?a:(x>b?b:x);} 
static float angle_to_target(const Game*g){return atan2f(target_x(g)-SHIP_X,SHIP_Y-target_y(g));}
static float normalize_angle(float a){while(a>PI)a-=2*PI;while(a< -PI)a+=2*PI;return a;}
void game_init(Game*g){
    memset(g,0,sizeof(*g));g->level=1;g->last_choice=-1;
    g->rng=(uint32_t)SDL_GetTicks()^0x7a83f197u;
    if(g->rng==0)g->rng=1;
    next_target(g);
}
static void miss(Game*g){
    g->errors++;g->error_flash=.75f;
    g->swarm_y=clampf(g->swarm_y-28.0f,-18.0f,190.0f);
}
void game_key(Game*g,SDL_Scancode sc){
    if(sc==SDL_SCANCODE_F1){game_init(g);return;}
    if(sc==SDL_SCANCODE_SPACE){g->paused=!g->paused;return;}
    if(sc==SDL_SCANCODE_TAB){g->easy=!g->easy;return;}
    if(g->paused||g->finished||g->firing)return;
    if(sc<SDL_SCANCODE_A||sc>SDL_SCANCODE_Z)return;
    char letter=(char)('A'+sc-SDL_SCANCODE_A);
    if(g->locked){
        if(letter=='F'){
            g->firing=true;
            audio_shoot();
            g->ship_angle=angle_to_target(g);
            g->shot_x=SHIP_X+26.0f*sinf(g->ship_angle);
            g->shot_y=SHIP_Y-26.0f*cosf(g->ship_angle);
            g->shot_target_x=target_x(g);g->shot_target_y=target_y(g);
            float dx=g->shot_target_x-g->shot_x,dy=g->shot_target_y-g->shot_y;
            float dist=sqrtf(dx*dx+dy*dy); if(dist<1)dist=1;
            g->shot_vx=dx/dist*690.0f;g->shot_vy=dy/dist*690.0f;
        } else miss(g);
        return;
    }
    if(letter==required_key(g)){
        /* Uma tecla digitada corresponde a UMA mira e UM disparo. */
        g->locked=true;
        g->success_flash=.16f;
    }else miss(g);
}
static void success(Game*g){
    spawn_particles(g,g->shot_target_x,g->shot_target_y,rgb(255,191,104));
    spawn_particles(g,g->shot_target_x,g->shot_target_y,rgb(255,234,171));
    g->success_flash=.50f;
    g->total++;
    /* Em palavras: contar um acerto por palavra, mas destruir um meteoro
       para CADA caractere (digitar -> F -> proxima letra). */
    if(g->level==3){
        g->typed++;
        if(g->target[g->typed]!='\0'){
            g->firing=false;
            g->locked=false;
            g->swarm_y=0;
            refresh_meteors(g);
            return;
        }
    }
    g->hits++;
    if(g->hits>=level_goal(g->level)){
        if(g->level==3){g->finished=true;g->firing=false;g->locked=false;return;}
        g->level++;g->hits=0;g->word_index=0;g->last_choice=-1;
        g->level_flash=2.6f;
    }
    next_target(g);
}
void game_update(Game*g,float dt){
    dt=clampf(dt,0,.05f);
    if(g->paused)return;
    g->time+=dt;
    if(g->error_flash>0)g->error_flash-=dt;
    if(g->success_flash>0)g->success_flash-=dt;
    if(g->level_flash>0)g->level_flash-=dt;
    for(int i=0;i<PARTICLE_MAX;i++){
        Particle*p=&g->particles[i];if(p->life<=0)continue;
        p->life-=dt;p->x+=p->vx*dt;p->y+=p->vy*dt;
        p->vx*=1.0f-0.7f*dt;p->vy*=1.0f-0.7f*dt;
    }
    if(g->finished)return;
    if(g->firing){
        g->shot_x+=g->shot_vx*dt;g->shot_y+=g->shot_vy*dt;
        float dx=g->shot_target_x-g->shot_x,dy=g->shot_target_y-g->shot_y;
        if(dx*dx+dy*dy<900 || (g->shot_vx*dx+g->shot_vy*dy)<=0)success(g);
        return;
    }
    float speed=(g->level==1?12.0f:g->level==2?16.5f:10.5f)+g->hits*1.10f;
    if(g->locked)speed*=.30f;
    if(g->easy)speed*=.65f;
    g->swarm_y+=speed*dt;
    if(g->swarm_y>38){g->missed++;g->swarm_y=0;} /* Nao perde progresso. */
    float desired=(g->locked||g->easy)?angle_to_target(g):0;
    float change=normalize_angle(desired-g->ship_angle);
    g->ship_angle+=clampf(change,-4.9f*dt,4.9f*dt);
}
