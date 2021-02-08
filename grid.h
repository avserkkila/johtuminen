#include <stdio.h>

#ifndef grid_h
typedef float gtyyp;
typedef struct {
  gtyyp* g;
  gtyyp* g2;
  int    gkoko;
  int*   Tmuutos;
  int*   jakoind;
  char * reuna;
  short* lohko; //reunapaikkojen lohot kiertojärjestyksessä
  short* okhol; //lohot takaperin kierrettynä
} grid_t;
#endif
#define grid_h

#define gridpit (resolx*resoly*resolz)
#define gridind(x,y,z) ((y)+resoly*(x)+resoly*resolx*(z))

grid_t grid;

void grid_asetaT(gtyyp* grd, float T, int koko);
void grid_jako();
int alkel_reunalla(int ind0);
void grid_Treun(gtyyp* grd);
void grid_Treun_2d(gtyyp* grd);
void ftallenna_grid   (FILE *f, float kerr, gtyyp* grd);
