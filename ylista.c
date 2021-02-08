#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ylista.h"

void tuhja(void*);

void* _yuusi_lista() {
  ylista* l = malloc(sizeof(ylista));
  if(!l)
    return NULL;
  l->seur = NULL;
  l->p = NULL;
  l->edel = NULL;
  return l;
}

/*lisää annetun jäsenen perään ja palauttaa lisätyn
  jos listaa ei ole, luodaan uusi*/
void* _ylisaa(void *l, void *ptr) {
  ylista* uusi = _yuusi_lista();
  if (!uusi) return NULL;
  uusi->p = ptr;
  
  if(!l) //ei ole valmista listaa joten palautetaan vain tämä
    return uusi;
  return _ylsvlms(l, uusi);
}

void* _ylsvlms(void *lv, void *jv) {
  ylista *l = (ylista*)lv;
  ylista *j = (ylista*)jv;
  if(!l)
    return j;
  /*jos on olemassa seuraava jäsen, se linkitetään tähän*/
  if(l->seur) {
    l->seur->edel = j;
    j->seur       = l->seur;
  }
  l->seur = j;
  j->edel = l;
  return j;
}

void* _yjatka(void* lv) {
  ylista* l = (ylista*) lv;
  return _ylsvlms(l, _yuusi_lista());
}

void* _ynouda(void* lv, int nro) {
  ylista *l = (ylista*)lv;
  if (nro < 0) {
    while(nro++ && (l = l->edel));
    return l;
  }
  while(nro-- && (l = l->seur));
  return l;
}

void* _yalkuun(void* lv) {
  ylista *l = (ylista*)lv;
  if(l)
    while (l->edel)
      l = l->edel;
  return l;
}

void* _yloppuun(void* lv) {
  ylista *l = (ylista*)lv;
  if(l)
    while (l->seur)
      l = l->seur;
  return l;
}

int _ylaske(void* lv) {
  ylista *l = (ylista*)lv;
  if (!l)
    return 0;
  int r = 1;
  while((l = l->seur))
    r++;
  return r;
}

int _yind(void* lv) {
  if(!lv)
    return -1;
  ylista* juokseva = _yalkuun(lv);
  int r = 0;
  while(juokseva != lv) {
    r++;
    juokseva = juokseva->seur;
  }
  return r;
}

/*palauttaa jäljelle jääneen listan ensimmäisen jäsenen*/
void* _ypoista(void* vl, int* nrot, int kpl, void (*vapautus)(void*)) {
  ylista* l = (ylista*)vl;
  void* r = _yalkuun(l);
  ylista *poistot[kpl];
  for(int i=0; i<kpl; i++)
    poistot[i] = _ynouda(l, nrot[i]);
  /*Jos poistetaan alku, johon r osoittaa, siirretään r uuteen alkuun*/
  for(int i=0; i<kpl; i++) {
    if (poistot[i] == r)
      r = _ypoista1(poistot[i], vapautus);
    else
      _ypoista1(poistot[i], vapautus);
  }
  return r;
}

void _yrm(void* lv, int* nrot, int kpl) {
  _ypoista((ylista*)lv, nrot, kpl, tuhja);
}

void _yrm1(void* pv) {
  _ypoista1((ylista*)pv, tuhja);
}

void _yrma(void* pv) {
  _ypoista_kaikki((ylista*)pv, tuhja);
}

/*Poistaa annetun jäsenen ja palauttaa seuraavan*/
void* _ypoista1(void* vptr, void (*vapautus)(void*) ) {
  ylista* ptr = (ylista*)vptr;
  ylista* r = ptr->seur;
  if (ptr->edel)
    ptr->edel->seur = ptr->seur;
  if (ptr->seur)
    ptr->seur->edel = ptr->edel;
  vapautus(ptr);
  free(ptr);
  return r;
}

void _ypoista_kaikki(void* vptr, void (*vapautus)(void*)) {
  while(vptr)
    vptr = _ypoista1(vptr, vapautus);
}

void _ytulosta(void *vl, char* muoto) {
  ylista* l = (ylista*)vl;
  if (strstr(muoto,"%s"))
    {
      while(l) {
	printf(muoto, (char*)l->p);
	l=l->seur;
      }
    }
  else
    {
      while(l) {
	printf(muoto, *(int*)(l->p));
	l=l->seur;
      }
    }
}

void _yftulosta(FILE *f, void *vl, char* muoto) {
  ylista* l = (ylista*)vl;
  if (strstr(muoto,"%s"))
    {
      while(l) {
	fprintf(f, muoto, (char*)l->p);
	l=l->seur;
      }
    }
  else
    {
      while(l) {
	fprintf(f, muoto, *(int*)(l->p));
	l=l->seur;
      }
    }
}

void _ytulostaf(void *vl, char* muoto, int x) {
  ylista* l = (ylista*)vl;
  if(strstr(muoto,"%s"))
    {
      while(l) {
	for(int i=0; (i<x && l); i++) {
	  printf(muoto, (char*)l->p);
	  putchar(' ');
	  l=l->seur;
	}
	putchar('\n');
      }
    }
  else
    {
      while(l) {
	for(int i=0; (i<x && l); i++) {
	  printf(muoto, *(int*)(l->p));
	  putchar(' ');
	  l=l->seur;
	}
	putchar('\n');
      }
    }
}

void tuhja(void* v) {
  ;
}
