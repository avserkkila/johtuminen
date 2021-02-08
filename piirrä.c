#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "piirrä.h"
#include "cfgmuuttujat.h"
#include "askellus.h"
#include "grid.h"

#define PI 3.14159265
#define vihrea1(c) round(sin((c)/255.0*PI)*255) //puolikas siniaalto
#define vihrea2(c) abs(round( pow(sin( (c) / 255.0 * PI*10 ), 2)*255)) //kymmenen sinipuolikasta
#define vihrea2b(c) abs(round( pow(sin( (c) / 255.0 * PI*15 ), 10)*255)) //kymmenen sinipuolikasta
#define vihrea3(c) floor(abs(c - 255/2.0)*2) //lineaarinen, 0 keskellä
#define vihrea4(c) abs(((c < 255/2)? 0 : 255) - c) //lineaarinen, 0 reunoilla
#define vihrea5(c) round(tanh( abs(((c < 255/2)? 0 : 255) - c)/80.0 )*255) //pieni jakaja korostaa reunoja
#define vihrea6(c) 0
#define aseta_vari(r,c) SDL_SetRenderDrawColor(r, c, vihrea5(c), 255-(c), 255)
#define aseta_vari2(r,c) SDL_SetRenderDrawColor(r, c, vihrea2b(c), 255-(c), 255)
#define palkkileveys 15

void kuvan_kertoimet(struct kuvamaareet *k) {
  k->varikerr = 255.0/(Tmax-Tmin);
  if(ndim == 1) {
    k->kokokerr.y = ikkuna_h*1.0/resoly;
    k->kokokerr.x = ikkuna_w;
  } else {
    k->kokokerr.x = (ikkuna_w*1.0/resolx < ikkuna_h*1.0/resoly) ?	\
      ikkuna_w*1.0/resolx : ikkuna_h*1.0/resoly;
    k->kokokerr.y = k->kokokerr.x;
  }
}

int lue_tekstuurit3(kuvamaareet_t *k) {
  int montako = 1; //muistakaani vaihtaa tarvittaessa
  int paikat_n = 2;
  SDL_Surface *lataus;
  k->tksrit3 = calloc(montako+1, sizeof(void*));

  if( !(lataus = SDL_LoadBMP("liuku.bmp")) ) {
    fprintf(stderr, "Virhe: ei luettu kuvaa liuku.bmp\n");
    return 1;
  }
  k->tksrit3[liuku] = SDL_CreateTextureFromSurface(k->rend, lataus);
  SDL_FreeSurface(lataus);

  /*paikkasuorakulmioitten alustaminen*/
  k->paikat3 = calloc(paikat_n + 1, sizeof(SDL_Rect));
  
  return 0;
}

void tuhoa_tekstuurit3(kuvamaareet_t *k) {
  if(!k->tksrit3)
    goto PAIKAT;
  for(int i=0; k->tksrit3[i]; i++) {
    SDL_DestroyTexture(k->tksrit3[i]);
    k->tksrit3[i] = NULL;
  }
  free(k->tksrit3);
  k->tksrit3 = NULL;

 PAIKAT:
  if(!k->paikat3)
    return;
  free(k->paikat3);
  k->paikat3 = NULL;
}

void piirra(gtyyp* grd, struct kuvamaareet *k) {
  SDL_SetRenderDrawColor(k->rend, 0, 0, 0, 255);
  SDL_RenderClear(k->rend);
  SDL_RenderSetScale(k->rend, k->kokokerr.x, k->kokokerr.y);

  for(int i=0; i<resolx; i++)
    for(int j=0; j<resoly; j++) {
      unsigned char variarvo = k->varikerr*(grd[gridind(i,j,0)]-Tmin);
      aseta_vari(k->rend, variarvo);
      SDL_RenderDrawPoint(k->rend, k->piirto_x0+i, k->piirto_y0+j);
    }
  
  if(ndim > 1) {
    varipalkki(k, resolx*k->kokokerr.x + 2*palkkileveys);
    if(ndim > 2)
      asiat3(k);
  }
  SDL_RenderPresent(k->rend);
}

int fpiirra(struct kuvamaareet *k) {
  if (fgetc(k->f) == EOF)
    return EOF;
  fseek(k->f, -1, SEEK_CUR);

  SDL_SetRenderDrawColor(k->rend, 0, 0, 0, 255);
  SDL_RenderClear(k->rend);
  SDL_RenderSetScale(k->rend, k->kokokerr.x, k->kokokerr.y);

  for(int i=0; i<resolx; i++)
    for(int j=0; j<resoly; j++) {
      unsigned char taso = fgetc(k->f);
      aseta_vari(k->rend, taso);
      SDL_RenderDrawPoint(k->rend, k->piirto_x0+i, k->piirto_y0+j);
    }
  varipalkki(k, resolx*k->kokokerr.x + 2*palkkileveys);

  if(k->g) {
    int i0 = ((ndim == 3)? 5 : 3) * palkkileveys + resolx;
    for(int i=0; i<resolx; i++)
      for(int j=0; j<resoly; j++) {
	unsigned char taso = fgetc(k->g);
	aseta_vari2(k->rend, taso);
	SDL_RenderDrawPoint(k->rend, i+i0, j);
      }
    varipalkki(k, 2*resolx*k->kokokerr.x + 5*palkkileveys);
  }
  
  if(ndim == 3)
    asiat3(k);
  SDL_RenderPresent(k->rend);
  return 0;
}

void asiat3(kuvamaareet_t *k) {
  SDL_RenderSetScale(k->rend, palkkileveys, 1);

  SDL_Rect *lrec = k->paikat3+ztasoliuku;
  *lrec = (SDL_Rect)				\
    {(resolx*k->kokokerr.x)/palkkileveys+4, 10, 1, resoly*k->kokokerr.y-20};
  if(lrec->h > ikkuna_h-lrec->y)
    lrec->h = ikkuna_h-lrec->y;
  
  k->paikat3[ztasovalitsin] = (SDL_Rect)	\
    {lrec->x, lrec->y+lrec->h, 1, 10};
  k->paikat3[ztasovalitsin].y -= \
    round((1.0)*k->paikat3[ztasovalitsin].h / 2 + lrec->h * k->ztaso*1.0/(resolz-1));

  SDL_RenderCopy(k->rend, k->tksrit3[liuku], NULL, k->paikat3+ztasoliuku);
  SDL_SetRenderDrawColor(k->rend, 250, 30, 30, 0);
  SDL_RenderFillRect(k->rend, k->paikat3+ztasovalitsin);
}

void varipalkki(kuvamaareet_t *k, int i) {
  SDL_RenderSetScale(k->rend, palkkileveys, ikkuna_h/255);
  i /= palkkileveys;
  for(int j=0; j<256; j++) {
    aseta_vari(k->rend, j);
    SDL_RenderDrawPoint(k->rend, i, 260-j);
  }			
}
