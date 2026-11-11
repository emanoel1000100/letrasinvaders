#include "game.h"
#include <math.h>

/* Mapa QWERTY com as 26 letras: fase 2 usa ambas as maos. */
const Key keys[KEY_COUNT]={
 {'Q',207,667,0},{'W',260,667,1},{'E',313,667,2},{'R',366,667,3},{'T',419,667,3},
 {'Y',472,667,4},{'U',525,667,4},{'I',578,667,5},{'O',631,667,6},{'P',684,667,7},
 {'A',228,725,0},{'S',281,725,1},{'D',334,725,2},{'F',387,725,3},{'G',440,725,3},
 {'H',493,725,4},{'J',546,725,4},{'K',599,725,5},{'L',652,725,6},
 {'Z',257,783,0},{'X',310,783,1},{'C',363,783,2},{'V',416,783,3},{'B',469,783,3},
 {'N',522,783,4},{'M',575,783,4}
};
const SDL_Color finger_colors[8]={
 {245,133,159,255},{248,195,102,255},{108,219,199,255},{147,172,253,255},
 {147,172,253,255},{108,219,199,255},{248,195,102,255},{245,133,159,255}
};
int key_index(char c){for(int i=0;i<KEY_COUNT;i++)if(keys[i].ch==c)return i;return -1;}
const char *finger_name(char c){
    static const char*names[]={"MINIMO E","ANELAR E","MEDIO E","INDICADOR ESQUERDO", "INDICADOR D","MEDIO D","ANELAR D","MINIMO D"};
    int i=key_index(c);return i>=0?names[keys[i].finger]:"INDICADOR ESQUERDO";
}
void draw_keyboard(SDL_Renderer*r,const Game*g){
    fill_rect(r,0,645,SCREEN_W,228,rgb(15,24,47));
    draw_line(r,0,644,SCREEN_W,644,rgb(63,89,138));
    draw_text(r,18,664,"TECLADO",2,rgb(199,215,246));
    draw_text(r,18,707,"ESQUERDA",2,rgb(132,217,200));
    draw_text(r,18,738,"DIREITA",2,rgb(174,179,245));
    draw_text(r,18,791,"F BASE",2,rgb(220,235,251));
    char wanted=required_key(g);
    int wanted_idx=key_index(wanted), f=key_index('F');
    if(wanted_idx>=0 && !g->locked){
        SDL_Color c=finger_colors[keys[wanted_idx].finger];
        draw_line(r,keys[f].x+23,keys[f].y+21,keys[wanted_idx].x+23,keys[wanted_idx].y+21,c);
    }
    for(int i=0;i<KEY_COUNT;i++){
        const Key *k=&keys[i];bool active=i==wanted_idx;
        SDL_Color fg=finger_colors[k->finger];
        bool flash=active && g->error_flash>0 && ((int)(g->error_flash*13)%2==0);
        SDL_Color bg=flash?rgb(255,255,255):(active?fg:rgb(39,53,84));
        fill_rect(r,k->x,k->y,47,44,bg);stroke_rect(r,k->x,k->y,47,44,fg);
        char label[2]={k->ch,0};
        draw_center(r,k->x+23,k->y+9,label,4,active?rgb(15,23,45):fg);
        if(k->ch=='F'||k->ch=='J')fill_rect(r,k->x+16,k->y+38,15,3,rgb(242,244,255));
    }
    draw_text(r,765,671,"F ATIRA",2,rgb(226,237,254));
    draw_text(r,765,704,"ESPACO PAUSA",2,rgb(187,204,234));
    draw_text(r,765,737,"TAB FACIL",2,rgb(187,204,234));
    draw_text(r,765,770,"F1 REINICIA",2,rgb(187,204,234));
    draw_text(r,765,803,"ESC SAIR",2,rgb(187,204,234));
}
