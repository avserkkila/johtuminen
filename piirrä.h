#include <stdio.h>
#include <SDL2/SDL.h>
#include "grid.h"

#ifndef piirra_h
typedef struct kuvamaareet {
  struct kokokerr_s {
    float x;
    float y;
  } kokokerr;
  int piirto_x0;
  int piirto_y0;
  float varikerr;
  int ztaso;
  SDL_Window* ikkuna;
  SDL_Renderer* rend;
  FILE *f;
  FILE *g;
  SDL_Texture **tksrit3;
  SDL_Rect *paikat3;
} kuvamaareet_t;

typedef enum {
  liuku
} tksri3_t;

typedef enum {
  ztasoliuku,
  ztasovalitsin
} paikka3_t;
#endif
#define piirra_h

void kuvan_kertoimet(struct kuvamaareet*);
int lue_tekstuurit3(kuvamaareet_t*);
void tuhoa_tekstuurit3(kuvamaareet_t*);

void piirra(gtyyp* grd, struct kuvamaareet*);
int fpiirra(struct kuvamaareet*);
void asiat3(struct kuvamaareet *k);
void varipalkki(kuvamaareet_t *k, int i);
