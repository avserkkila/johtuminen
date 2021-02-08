#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "grid.h"
#include "cfgmuuttujat.h"

/*kulmilla tehdään vain jakolaskuja, joten kulmat supistuvat pois*/
const float pi = 1;

void asetamuisti(short* p, int arvo, int kpl) {
  for(int i=0; i<kpl; i++)
    p[i] = arvo;
}

void grid_asetaT(gtyyp* grd, float T, int koko) {
  for(int i=0; i<koko; i++)
    grd[i] = T;
}

void grid_jako() {
  /*määritetään jakolinjojen sijainnit
    ensin neliössä ja sitten huomioidaan anisotropia*/
  if (nens == 0 || ndim == 1) {
    grid.reuna    = NULL;
    return;
  }
  int umparus = 2*resolx+2*resoly-4;
  grid.reuna   = calloc(nens,    sizeof(char));
  grid.lohko   = calloc(umparus, sizeof(short));
  grid.okhol   = calloc(umparus, sizeof(short));
  grid.jakoind = calloc(nens+1,  sizeof(int));
  grid.jakoind[nens] = -1;
  
  double Dfi = 2*pi / nens; //lohon pituus radiaaneina
  /*Neliössä sivun pituus olisi pi/2 */
  double fi = pi/2*jakosrt; //juoksevan kulman alustus

  while(fi < 0) //tämä mahdollistaa negatiivisen jakosiirron
    fi += 2*pi;
  
  for(int k=0; k<nens; k++) {
    while(fi > 2*pi)
      fi -= 2*pi;

    short paikkar;
    /*katsotaan mille reunalle osuu ja haetaan sieltä kohta*/
    /*lohot siten että ensin 0 kaikkiin,
      sitten 1. jakolinjalta 1 0:n alkuun,
      sitten seur linjalta 2 0: alkuun jne*/
    //tässä kaikki, mitä tehdään koskee pelkästään reunoja paitsi jakoind
    int alku;
    if((0 <= fi && fi < pi/2) || fi > 2*pi) {
      if(fi > 2*pi)
	fi -= 2*pi;
      paikkar = ceil( (resolx-1)*(fi-0     )/(pi/2) -0.0001);
      grid.reuna[k] = 1;
      grid.jakoind[k] = gridind(paikkar, 0, 0);
      if(k==0) alku = paikkar;
      asetamuisti(grid.lohko+paikkar, k, umparus-paikkar); //samaa loppuun
      asetamuisti(grid.lohko, k, alku); //samaa ensimmäisen alkuun

    } else if(pi/2 <= fi && fi < pi) {
      paikkar = ceil( (resoly-1)*(fi-pi/2  )/(pi/2) -0.0001);
      grid.jakoind[k] = gridind(resolx-1, paikkar, 0);
      paikkar += resolx-1; //siirto oikealle reunalle
      grid.reuna[k] = 2;
      if(k==0) alku = paikkar;
      asetamuisti(grid.lohko+paikkar, k, umparus-paikkar);
      asetamuisti(grid.lohko, k, alku);

    } else if(pi   <= fi && fi < 3*pi/2) {
      paikkar = ceil( (resolx-1)*(fi-pi    )/(pi/2) -0.0001);
      grid.jakoind[k] = gridind(resolx-1-paikkar, resoly-1, 0);
      grid.reuna[k] = 3;
      paikkar += umparus/2;
      if(k==0) alku = paikkar;
      asetamuisti(grid.lohko+paikkar, k, umparus-paikkar);
      asetamuisti(grid.lohko, k, alku);

    } else {
      paikkar = ceil( (resoly-1)*(fi-3*pi/2)/(pi/2) -0.0001);
      grid.jakoind[k] = gridind(0, resoly-1-paikkar, 0);
      grid.reuna[k] = 4;
      paikkar += (umparus-resoly+2);
      if(k==0) alku = paikkar;
      asetamuisti(grid.lohko+paikkar, k, umparus-paikkar);
      asetamuisti(grid.lohko, k, alku);
    }
    fi += Dfi;
  }
  for(int i=0; i<umparus; i++)
    grid.okhol[i] = grid.lohko[umparus-1-i];
}

int askel_reunalla(int ind0) {
  /*oikea ylänurkka*/
  if(ind0 == (resolx-1)*resoly)
    return ind0+1;
  /*vasen alanurkka*/
  if(ind0 == resoly-1)
    return ind0-1;
  /*yläreuna*/
  if(ind0 % resoly == 0)
    return ind0+resoly;
  /*alareuna*/
  if((ind0+1) % resoly == 0)
    return ind0-resoly;
  /*vasen*/
  if(ind0 < resoly)
    return ind0-1;
  /*oikea*/
  if(ind0 > (resolx-1)*resoly)
    return ind0+1;
  return -1; //ei ollut reunalla;
}

void grid_Treun(gtyyp* grd) {
  switch(ndim) {
  case 1:
    grd[0] = Treun[0];
    grd[resoly-1] = Treun[1];
    break;
  case 2:
    grid_Treun_2d(grd);
    break;
  case 3:
    /*vakiolämmöt ylä- ja alapinnoille eli z-tasot*/
    grid_asetaT( grd, Tala, resolx*resoly );
    grid_asetaT( grd+gridind(0,0,resolz-1), Tula, resolx*resoly );

    /*laajennetaan reunat pystylaidoiksi eli x- ja y-tasot*/
    for(int k=1; k<resolz-1; k++)
      grid_Treun_2d(grd+gridind(0,0,k));
    break;
  }
}

void grid_Treun_2d(gtyyp* grd) {
  int ind   = grid.jakoind[0];
  int lohko = 0;
  do {
    grd[ind] = Treun[lohko];
    if( (ind = askel_reunalla(ind)) < 0)
      fprintf(stderr, "Virhe: indeksi ei ollut alueen reunalla asetettaessa reunan lämpötiloja\n");
    if(ind == grid.jakoind[lohko+1])
      lohko++;
  } while(ind != grid.jakoind[0]);
}

void ftallenna_grid(FILE *f, float kerr, gtyyp* grd) {
  for(int i=0; i<resolx*resoly*tallresolz; i++) {
    unsigned char c = kerr*(grd[i]-Tmin);
    fputc(c, f);
  }
}
