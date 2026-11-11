#ifndef LETRAS_GAME_H
#define LETRAS_GAME_H
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdint.h>

#define SCREEN_W 960
#define SCREEN_H 873
#define SHIP_Y 591.0f
#define PARTICLE_MAX 110
#define KEY_COUNT 26
#define TARGET_MAX 16
#define METEOR_COUNT 8

typedef struct {float x,y,vx,vy,life; SDL_Color color;} Particle;
typedef struct {char ch; int x,y,finger;} Key;
typedef struct {
    int level, hits, total, errors, missed, word_index;
    bool finished, paused, easy, locked, firing;
    char target[TARGET_MAX];
    char meteor_letters[METEOR_COUNT];
    float meteor_pos_x[METEOR_COUNT], meteor_pos_y[METEOR_COUNT];
    int meteor_count;
    int typed, selected, last_choice;
    float swarm_y, time, error_flash, success_flash, level_flash;
    float ship_angle, shot_x, shot_y, shot_vx, shot_vy;
    float shot_target_x, shot_target_y;
    Particle particles[PARTICLE_MAX];
    uint32_t rng;
} Game;

extern const Key keys[KEY_COUNT];
extern const SDL_Color finger_colors[8];

SDL_Color rgb(int r,int g,int b);
void fill_rect(SDL_Renderer*r,int x,int y,int w,int h,SDL_Color c);
void stroke_rect(SDL_Renderer*r,int x,int y,int w,int h,SDL_Color c);
void draw_line(SDL_Renderer*r,int x1,int y1,int x2,int y2,SDL_Color c);
void fill_circle(SDL_Renderer*r,int cx,int cy,int radius,SDL_Color c);
void stroke_circle(SDL_Renderer*r,int cx,int cy,int radius,SDL_Color c);
void draw_text(SDL_Renderer*r,int x,int y,const char*s,int scale,SDL_Color c);
void draw_center(SDL_Renderer*r,int x,int y,const char*s,int scale,SDL_Color c);
int text_width(const char *s,int scale);

int key_index(char c);
const char *finger_name(char c);
void draw_keyboard(SDL_Renderer*r,const Game*g);

int level_goal(int level);
void next_target(Game *g);
void refresh_meteors(Game *g);
char required_key(const Game*g);

bool audio_init(void);
void audio_shoot(void);
void audio_pause(bool paused);
void audio_shutdown(void);

void game_init(Game*g);
void game_update(Game*g,float dt);
void game_key(Game*g,SDL_Scancode key);
void spawn_particles(Game*g,float x,float y,SDL_Color c);
float target_x(const Game*g);
float target_y(const Game*g);
float meteor_x(const Game*g,int idx);
float meteor_y(const Game*g,int idx);
void render_game(SDL_Renderer*r,const Game*g);

#endif
