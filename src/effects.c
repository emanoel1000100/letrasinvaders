#include "game.h"
#include <math.h>
static unsigned rand32(Game*g){uint32_t n=g->rng;n^=n<<13;n^=n>>17;n^=n<<5;g->rng=n;return n;}
void spawn_particles(Game*g,float x,float y,SDL_Color c){
    int added=0;
    for(int i=0;i<PARTICLE_MAX && added<45;i++){
        if(g->particles[i].life>0)continue;
        float angle=(float)(rand32(g)%6283)/1000.0f;
        float speed=50.0f+(float)(rand32(g)%145);
        Particle*p=&g->particles[i];
        p->x=x;p->y=y;p->vx=cosf(angle)*speed;p->vy=sinf(angle)*speed;
        p->life=.45f+(float)(rand32(g)%45)/100.0f;p->color=c;
        added++;
    }
}
