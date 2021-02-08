#ifndef ylista_h
typedef struct ylista_s {
  struct ylista_s *edel;
  void *p;
  struct ylista_s *seur;
} ylista;
#endif
#define ylista_h

void* _yuusi_lista();
void* _ylisaa(void* l, void* j);
void* _ylsvlms(void* lista, void* jasen);
void* _yjatka(void*);
void* _ynouda(void*, int);
void* _ypoista(void*, int*, int, void (*f)(void*));
void* _ypoista1(void*, void (*f)(void*));
void  _ypoista_kaikki(void* ptr, void (*f)(void*));
void  _yrm(void*, int*, int);
void  _yrm1(void* j);
void  _yrma(void*);
void* _yalkuun(void*);
void* _yloppuun(void*);
int   _ylaske(void*);
int   _yind(void*);
void  _ytulosta(void* lista, char* muoto);
void  _yftulosta(FILE *f, void* lista, char* muoto);
void  _ytulostaf(void*, char* muoto, int x);
void tuhja(void*);
