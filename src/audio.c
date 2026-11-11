#include "game.h"
#include <math.h>
#include <string.h>

/* Original procedural arcade loop; no files or extra audio dependencies. */
#define SAMPLE_RATE 48000
#define SHOT_SAMPLES 9600
#define TAU 6.28318530717958647692
#define MUSIC_VOLUME 0.045f
#define SHOT_VOLUME 0.14f

typedef struct {
    uint64_t frame;
    int shot_left;
    double lead_phase,bass_phase,shot_phase;
    bool paused;
} AudioState;
static AudioState state;
static SDL_AudioDeviceID device;

static void audio_mix(AudioState *s,float *out,int count){
    static const int melody[32]={76,79,83,79,74,78,81,78,72,76,79,76,74,78,81,86,
                                76,79,83,86,74,78,81,78,72,76,79,84,74,78,81,74};
    static const int bass[4]={40,38,36,38};
    const int step_samples=7200;
    for(int i=0;i<count;i++){
        if(s->paused){out[i]=0;continue;}
        unsigned step=(unsigned)((s->frame/step_samples)%32);
        double beat=(double)(s->frame%step_samples)/step_samples;
        double lead_hz=440.0*pow(2.0,(melody[step]-69)/12.0);
        double bass_hz=440.0*pow(2.0,(bass[step/8]-69)/12.0);
        s->lead_phase+=lead_hz/SAMPLE_RATE;s->lead_phase-=floor(s->lead_phase);
        s->bass_phase+=bass_hz/SAMPLE_RATE;s->bass_phase-=floor(s->bass_phase);
        /* Soft-edged chiptune harmonics, short attacks prevent clicks. */
        double envelope=fmin(beat*40.0,1.0)*fmax(0.0,1.0-beat);
        double lead=(sin(TAU*s->lead_phase)+0.25*sin(3*TAU*s->lead_phase))/1.25;
        double low=sin(TAU*s->bass_phase);
        double pulse=sin(TAU*85.0*(s->frame%step_samples)/SAMPLE_RATE)*exp(-beat*20.0);
        float sample=MUSIC_VOLUME*(float)(0.55*lead*envelope+0.30*low+0.15*pulse);
        if(s->shot_left>0){
            double t=(double)(SHOT_SAMPLES-s->shot_left)/SHOT_SAMPLES;
            double hz=1700.0*pow(0.09,t);
            s->shot_phase+=hz/SAMPLE_RATE;s->shot_phase-=floor(s->shot_phase);
            double env=fmin(t*100.0,1.0)*(1-t)*(1-t);
            sample+=SHOT_VOLUME*(float)(env*(sin(TAU*s->shot_phase)+0.22*sin(2*TAU*s->shot_phase))/1.22);
            s->shot_left--;
        }
        out[i]=sample;s->frame++;
    }
}
static void audio_callback(void *userdata,Uint8 *stream,int len){
    audio_mix((AudioState*)userdata,(float*)stream,len/(int)sizeof(float));
}
bool audio_init(void){
    if(device)return true;
    if(SDL_InitSubSystem(SDL_INIT_AUDIO)!=0)return false;
    SDL_AudioSpec wanted={0};
    wanted.freq=SAMPLE_RATE;wanted.format=AUDIO_F32SYS;wanted.channels=1;
    wanted.samples=512;wanted.callback=audio_callback;wanted.userdata=&state;
    memset(&state,0,sizeof state);
    device=SDL_OpenAudioDevice(NULL,0,&wanted,NULL,0);
    if(!device){SDL_QuitSubSystem(SDL_INIT_AUDIO);return false;}
    SDL_PauseAudioDevice(device,0);return true;
}
void audio_shoot(void){
    if(!device)return;
    SDL_LockAudioDevice(device);
    state.shot_left=SHOT_SAMPLES;state.shot_phase=0;
    SDL_UnlockAudioDevice(device);
}
void audio_pause(bool paused){
    if(!device)return;
    SDL_LockAudioDevice(device);state.paused=paused;SDL_UnlockAudioDevice(device);
}
void audio_shutdown(void){
    if(!device)return;
    SDL_CloseAudioDevice(device);device=0;SDL_QuitSubSystem(SDL_INIT_AUDIO);
}
