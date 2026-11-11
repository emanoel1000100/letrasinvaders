#include "game.h"
#include <stdio.h>
#include <string.h>

/* Todas as palavras usam somente as 15 teclas da mao esquerda. */
static const char *words[]={
 "CASA","FACA","SEDE","REDE","FESTA","CARTA","BASE","BRASA",
 "TRAVA","FACE","VERDE","TARDE","VACA","CAFE","FARDA","SERRA",
 "XADREZ","ZEBRA","EXTRA","AZEDA","GRAVE","BREVE","FRASE","ABACATE"
};
static const char *row1="ASDFG";
static const char *alphabet="ABCDEFGHIJKLMNOPQRSTUVWXYZ";
static const char *all_left="ASDFGQWERTZXCVB";

static unsigned random_next(Game*g){
    uint32_t a=g->rng;a^=a<<13;a^=a>>17;a^=a<<5;g->rng=a? a:1;return g->rng;
}
static char random_char(Game*g,const char*pool,char avoid){
    int n=(int)strlen(pool); if(n<=0)return 'A';
    char ch=pool[(int)(random_next(g)%(unsigned)n)];
    if(n==1)return ch;
    while(ch==avoid) ch=pool[(int)(random_next(g)%(unsigned)n)];
    return ch;
}
static const char* current_pool(const Game*g){
    if(g->level==1) return row1;
    if(g->level==2) return alphabet;
    return all_left;
}
int level_goal(int level){return level==3?8:(level==2?26:15);}
char required_key(const Game*g){
    if(g->locked)return 'F';
    return g->target[g->typed] ? g->target[g->typed] : 'F';
}
void refresh_meteors(Game*g){
    const char *pool=current_pool(g);
    char wanted=g->target[g->typed];
    /* Cada nova letra cria um novo enxame de 4 a 8 meteoros. */
    g->meteor_count=4+(int)(random_next(g)%5);
    g->selected=(int)(random_next(g)%(unsigned)g->meteor_count);
    int locations[8]={0,1,2,3,4,5,6,7};
    for(int i=7;i>0;i--){
        int j=(int)(random_next(g)%(unsigned)(i+1));
        int temp=locations[i];locations[i]=locations[j];locations[j]=temp;
    }
    for(int i=0;i<g->meteor_count;i++){
        int slot=locations[i];
        /* Nasce ABAIXO do titulo e da palavra, nunca dentro da barra superior. */
        g->meteor_pos_x[i]=125.0f+(slot%4)*236.0f+(float)((int)(random_next(g)%35)-17);
        g->meteor_pos_y[i]=222.0f+(slot/4)*105.0f+(float)((int)(random_next(g)%25)-12);
        g->meteor_letters[i]=(i==g->selected)?wanted:random_char(g,pool,wanted);
    }
}
void next_target(Game*g){
    int pick=0, pool_len=0;
    const char *pool=row1;
    if(g->level==3){
        pool_len=(int)(sizeof(words)/sizeof(words[0]));
        static const int learning_order[]={0,1,6,7,16,17,18,8};
        pick=(g->word_index<8)?learning_order[g->word_index]:(g->word_index*7+3)%pool_len;
        if(g->word_index==0 && g->last_choice==pick)pick=(pick+1)%pool_len;
        snprintf(g->target,sizeof g->target,"%s",words[pick]);
        g->word_index++;
    }else{
        pool=(g->level==2)?alphabet:row1;
        pool_len=(int)strlen(pool);
        /* Fase 2: percorre A-Z, uma vez cada, em ordem embaralhada fixa. */
        if(g->level==2)pick=(g->hits*11+7)%26;
        else{
            pick=(int)(random_next(g)%(unsigned)pool_len);
            if(g->last_choice==pool[pick])pick=(pick+1)%pool_len;
        }
        g->target[0]=pool[pick];g->target[1]=0;
    }
    g->last_choice=g->level==3?pick:g->target[0];
    g->typed=0;g->locked=false;g->firing=false;
    g->swarm_y=0;
    g->error_flash=0;
    refresh_meteors(g);
}
