#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "cfgmuuttujat.h"
#include "piirrä.h"
#include "askellus.h"
#include "käyttöliittymä.h"
#include "grid.h"

void tiedoston_alustus(FILE *f) {
  fwrite(&ndim,       sizeof(int), 1, f);
  fwrite(&resolx,     sizeof(int), 1, f);
  fwrite(&resoly,     sizeof(int), 1, f);
  fwrite(&tallresolz, sizeof(int), 1, f);
  fwrite(&Tmin,     sizeof(float), 1, f);
  fwrite(&Tmax,     sizeof(float), 1, f);
}

void luennan_alustus(FILE* f) {
  fread(&ndim,   sizeof(int),   1, f);
  fread(&resolx, sizeof(int),   1, f);
  fread(&resoly, sizeof(int),   1, f);
  fread(&resolz, sizeof(int),   1, f);
  fread(&Tmin,   sizeof(float), 1, f);
  fread(&Tmax,   sizeof(float), 1, f);
}

int main(int argc, char** argv) {
  time_t nollaaika;
  time_t alkuaika;
  clock_t alkucpu;
  time(&nollaaika);
  int r = 0;
  struct kuvamaareet kuva;
  if(argc == 1)
    {
      /*gridin luominen*/
      if (cfgmuuttujat("ajotapa grid kuvatiedot lämpötilat"))
	return 1;
      grid.g = calloc(gridpit, sizeof(gtyyp));
      grid_asetaT(grid.g, Tin, gridpit);
      grid_jako();
      grid_Treun(grid.g);
      
      /*iterointia varten toinen sama ja apumuuttuja*/
      grid.g2 = calloc(gridpit, sizeof(gtyyp));
      if(!grid.g2) return 1;
      for(int i=0; i<gridpit; i++)
	grid.g2[i] = grid.g[i];

      /*tallennuksen valmistelu*/
      if(tallenna) {
	if(!(kuva.f = fopen(tall_nimi, "wb")))
	  return 1;
	tiedoston_alustus(kuva.f);
	
      if(!piirto_heti)
	goto AJA;
      }
    }
  else
    {
      /*ulostulon katsominen*/
      if(cfgmuuttujat("ajotapa kuvatiedot"))
	return 1;
      if(!(kuva.f = fopen(argv[1], "rb"))) {
	fprintf(stderr, "Virhe: ei tiedostoa \"%s\"\n", argv[1]);
	return 1;
      }
      if(argc > 2) {
	if(!(kuva.g = fopen(argv[2], "rb"))) {
	  fprintf(stderr, "Virhe: ei tiedostoa \"%s\"\n", argv[2]);
	}
      } else {
	kuva.g = NULL;
      }
	
      luennan_alustus(kuva.f);
    }
  /*ikkunaan liittyvät alustamiset*/
  if (SDL_Init(SDL_INIT_VIDEO)) {
    fprintf(stderr, "Ei voi alustaa SDL-grafiikkaa: %s\n", SDL_GetError());
    r = 1;
    goto EI_SDL;
  }
  kuva.ikkuna = SDL_CreateWindow\
    ("Lämmönjohtuminen",\
     ikkuna_x, ikkuna_y, ikkuna_w, ikkuna_h, SDL_WINDOW_RESIZABLE);
  kuva.rend = SDL_CreateRenderer(kuva.ikkuna, -1, SDL_RENDERER_TARGETTEXTURE);
  
  if(lue_tekstuurit3(&kuva))
    goto SULJE;

 AJA:
  kuvan_kertoimet(&kuva);
  alkucpu = clock();
  printf("Aika aloitettaessa: %li ms (CPU), %li s (oikea)\n",
	 alkucpu/(CLOCKS_PER_SEC/1000),
	 time(&alkuaika)-nollaaika);
  if(argc == 1)
    r = kaunnista(&kuva);
  else
    katso(&kuva);
  printf("\rAjoaika: %li ms (CPU), %li s (oikea)\n",
	 (clock()-alkucpu)/(CLOCKS_PER_SEC/1000),
	 time(&nollaaika)-alkuaika);

 SULJE:
  if(piirto_heti || argc != 1) {
    tuhoa_tekstuurit3(&kuva);
    SDL_DestroyRenderer(kuva.rend);
    SDL_DestroyWindow(kuva.ikkuna);
    SDL_Quit();
  }

 EI_SDL:
  if(argc == 1) {
    free(grid.g);
    free(grid.g2);
    free(grid.jakoind);
    free(grid.reuna);
    free(grid.lohko);
  } else
    fclose(kuva.f);
  return r;
}
