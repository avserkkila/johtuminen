#include <stdio.h>
#include "ylista.h"

#ifndef lue_cfg_h
enum lue_cfg_virheet {
  ei_tiedostoa = 1,
  ei_osioa,
  osio_loppui,
  alustusvirhe,
  ei_luettu,
  jotain_outoa
};
#endif
#define lue_cfg_h

int lue_cfg1(FILE* f, void* p);
int lue_cfgosio(FILE* f, char* osio, ylista **vars);
int lue_cfg(char* osiot, ylista *vars);
int lue_cfgmuuttuja(char* osio, char* nimi, void* p);
