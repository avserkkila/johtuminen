#include <string.h>
#include "cfgmuuttujat.h"
#include "lue_cfg.h"

char cfgmuuttujat(char* osiot) {
  ylista* l = NULL;
  if(strstr(osiot, "ajotapa") || strstr(osiot, "kaikki")) {
    l = _ylisaa(l, &askel_max);
    l = _ylisaa(l, &piirto_heti);
    l = _ylisaa(l, &tallenna);
    l = _ylisaa(l, &tall_nimi);
    l = _ylisaa(l, &piirtoviive);
    l = _ylisaa(l, &piirtovali);
    l = _ylisaa(l, &tallvali);
    l = _ylisaa(l, &tul_lohot);
  }
  if(strstr(osiot, "grid") || strstr(osiot, "kaikki")) {
    l = _ylisaa(l, &ndim);
    l = _ylisaa(l, &resolx);
    l = _ylisaa(l, &resoly);
    l = _ylisaa(l, &resolz);
    l = _ylisaa(l, &tallresolz);
    l = _ylisaa(l, &tallkork);
    l = _ylisaa(l, &nens);
    l = _ylisaa(l, &jakosrt);
  }
  if(strstr(osiot, "kuvatiedot") || strstr(osiot, "kaikki")) {
    l = _ylisaa(l, &ikkuna_x);
    l = _ylisaa(l, &ikkuna_y);
    l = _ylisaa(l, &ikkuna_w);
    l = _ylisaa(l, &ikkuna_h);
  }
  if(strstr(osiot, "lämpötilat") || strstr(osiot, "kaikki")) {
    l = _ylisaa(l, &Tin);
    l = _ylisaa(l, &Treun);
    l = _ylisaa(l, &Tala);
    l = _ylisaa(l, &Tula);
    l = _ylisaa(l, &Tmin);
    l = _ylisaa(l, &Tmax);
    l = _ylisaa(l, &Tkkeski);
    l = _ylisaa(l, &Tkreun);
    l = _ylisaa(l, &Tkala);
    l = _ylisaa(l, &Tkula);
  }
  if(!l) return 1;
  l = _yalkuun(l);
  int r = lue_cfg(osiot, l);
  
  /*muokataan lämpötilan ääriarvot ja kerroin*/
  if(strstr(osiot, "lämpötilat") || strstr(osiot, "kaikki")) {
    if(Tmin >= Tmax) {
      Tmin = Tin;
      Tmax = Tin;
      int ularaja = (ndim == 1)? 2 : nens;
      
      for(int i=0; i<ularaja; i++)
	if(Treun[i] < Tmin) Tmin = Treun[i];
	else if(Treun[i] > Tmax) Tmax = Treun[i];
    }
    if(ndim == 3) {
      if(Tula > Tmax) Tmax      = Tula;
      else if(Tula < Tmin) Tmin = Tula;
      if(Tala > Tmax) Tmax      = Tala;
      else if(Tala < Tmin) Tmin = Tala;
    }
  }

  if(strstr(osiot, "grid") || strstr(osiot, "kaikki")) {
    if(ndim < 3)
      resolz = 1;
    if(ndim < 2)
      resolx = 1;
    if(tallkork < 0)
      tallkork = (resolz-1) / 2;
    if(resolz-tallkork < tallresolz || tallresolz < 0)
      tallresolz = resolz-tallkork;
  }
  
  _yrma(l);
  return r;
}
