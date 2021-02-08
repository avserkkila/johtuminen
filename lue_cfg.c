#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "lue_cfg.h"

const int yrite_l0 = 300;
const int apu_l0   = 30;
int yrite_l;
int apu_l;

int lue_cfg(char* osiot, ylista *vars) {
  FILE *osioStream = NULL;
  char* osio       = NULL;
  int r = 0;
  if( !(osioStream = fmemopen(osiot, strlen(osiot), "r")) )
    r = jotain_outoa;
  if( !(osio = malloc(strlen(osiot)+1)) )
    r = alustusvirhe;

  while (r == 0 && !feof(osioStream) && fscanf(osioStream, "%s", osio))
    r = lue_cfgosio(NULL, osio, &vars);
  switch (r)
    {
    case ei_tiedostoa:
      fprintf(stderr, "Virhe: ei tiedostoa \"cfg\"\n");
      break;
    case ei_osioa:
      fprintf(stderr, "Virhe: tiedostossa \"cfg\" ei ole osioa \"%s\"\n", osio);
      break;
    case alustusvirhe:
      fprintf(stderr, "Virhe: jokin alloc epäonnistui\n");
      break;
    case EOF:
      fprintf(stderr, "Virhe: EOF tiedostossa \"cfg\"\n");
      break;
    case ei_luettu:
      fprintf(stderr, "Virhe: jonkin muuttujan arvo ei ollut luettava, virhe-enum = ei_luettu\n");
      break;
    case jotain_outoa:
      fprintf(stderr, "Virhe: virhe-enum = jotain_outoa\n");
      break;
    case 0:
      if (vars != _yloppuun(vars))
	fprintf(stderr, "Virhe: osiosta \"%s\" ei löytynyt tarpeeksi muuttujan arvoja, osio loppui\n", osio);
      break;
    }
  free(osio);
  fclose(osioStream);
  return r;
}

int lue_cfgosio(FILE *f, char* osio, ylista** vars) {
  char tied_suljetaan = 0;
  char *apu, *yrite;
  if(!f) {
    tied_suljetaan = 1;
    if( !(f = fopen("cfg","r")) )
      return ei_tiedostoa;
  }
  
  yrite_l = yrite_l0 + strlen(osio);
  apu_l   = apu_l0 + strlen(osio);

  /*muistin alustus*/
  yrite_l += strlen(osio);
  apu_l = yrite_l;
  if( !(yrite = calloc(yrite_l+1,1))		\
      || !(apu = calloc(apu_l+1,1)) )
    return alustusvirhe;
  
  /*siirry osioon*/
  *apu = '%';
  strcat(apu, osio);
  while (fscanf(f, "%s", yrite) && !feof(f))
    if(!strcmp(yrite, apu)) 
      goto OSIO_LOUTUI;
  return ei_osioa;

  /*'='-merkin jälkeen kutsutaan lue_cfg1
    '#'-merkin jälkeen mennään seuraavalle riville
    tarkistetaan myös loppuiko luettava osio*/
 OSIO_LOUTUI:
  while(*vars && (*vars)->p) {
    fscanf(f, "%s", yrite);
    if (feof(f)) {
      return EOF;
    } else if (!strcmp(yrite, "=")) {
      int r;
      if((r = lue_cfg1(f, (*vars)->p))) {
	return r;
      }
      (*vars) = (*vars)->seur;
    } else if (yrite[0] == '#') {
      while (!feof(f) && fgetc(f) != '\n'); //seur. rivi
      continue;
    } else {
      strcpy(apu, "%\\");
      strcat(apu, osio);
      if(!strcmp(apu, yrite))
	break;
    }
  }
  if(tied_suljetaan)
    fclose(f);
  free(apu);
  free(yrite);
  return 0;
}
  
int lue_cfg1(FILE* f, void* p) {
  char *apu = calloc(50, 1);
  while( isspace(*apu = fgetc(f)) );
  fseek(f,-1,SEEK_CUR);
  
  int i = -1;
  char pointteri = 0;
  char kelluva = 0;
  char stringi = 0;
  /*tarkistetaan tyyppi*/
  while((apu[++i] = fgetc(f)) != ';') {
    switch(apu[i])
      {
      case '[':
	pointteri = 1;
	break;
      case '.':
	kelluva = 1;
	goto LUE_LOPUT_TARKISTAMATTA;
      case '\'':
      case '"':
	stringi = 1;
	goto LUE_LOPUT_TARKISTAMATTA;
      }
    if (i == apu_l)
      apu = realloc(apu, apu_l += apu_l0);
  }
 LUE_LOPUT_TARKISTAMATTA:
  while(apu[i] != ';') {
    if (i == apu_l)
      apu = realloc(apu, apu_l += apu_l0);
    apu[++i] = fgetc(f);
  }
  apu[++i] = '\0';

  /*luetaan apu, joka on nyt char-muodossa, sopivaksi muuttujaksi*/
  if(stringi)
    {
      if( !(*(char**)p = malloc(strlen(apu)+1))) return alustusvirhe;
      strcpy(*(char**)p, apu+1);
      (*(char**)p)[strlen(*(char**)p)-2] = '\0'; // lopusta pois lainausmerkki ja ';'
    }
  else if (kelluva && !pointteri)
    {
      if(!(sscanf(apu, "%g", (float*)p)))
	return ei_luettu;
    }
  else if(!kelluva && !pointteri)
    {
      if(!(sscanf(apu, "%i", (int*)p)))
	return ei_luettu;
    }
  else // pointteri
    {
	FILE *stream = fmemopen(apu+1, strlen(apu), "r");
	if (!stream)
	  return jotain_outoa;
	
	if (!kelluva) {
	  if( !(*(int**)p = malloc(sizeof(int)*2)))
	    return alustusvirhe;
	  int i=0;
	  while(!feof(stream)) {
	    if(fscanf(stream, "%i", (*(int**)p)+i)) {
	      i++;
	      *(int**)p = realloc(*(int**)p, sizeof(int)*(i+1));
	    } else
	    fgetc(stream);
	  }
	} else { // kelluva
	  if( !(*(float**)p = malloc(sizeof(float)*2)))
	    return alustusvirhe;
	  int i=0;
	  while(!feof(stream)) {
	    if(fscanf(stream, "%g", (*(float**)p)+i)) {
	      i++;
	      *(float**)p = realloc(*(float**)p, sizeof(float)*(i+1));
	    } else
	    fgetc(stream);
	  }
	}
	fclose(stream);
    }
  return 0;
}

int lue_cfgmuuttuja(char* osio, char* nimi, void* muuttuja) {
  FILE *f = fopen("cfg", "r");
  if(!f)
    return ei_tiedostoa;
  
  /*siirrytään osioon*/
  if(osio) {
    ylista* tuhja = _yuusi_lista();
    int r = lue_cfgosio(f, osio, &tuhja);
    free(tuhja);
    if(r) return r;
  }

  /*haetaan muuttuja*/
  char *yrite = calloc(strlen(nimi)+1,1);
  char *paattui;
  if(osio) {
    paattui = calloc(strlen(osio)+3, 1);
    strcat(strcpy(paattui, "%\\"), osio);
  } else {
    paattui = calloc(1,1);
  }
  if(!(yrite && paattui))
    return alustusvirhe;
  
  int r = 3000;
  while(r == 3000) {
    fscanf(f, "%s", yrite);
    if(feof(f))
      r = EOF;
    if(yrite[0] == '#')
      while(!feof(f) && fgetc(f) != '\n');
    else if(!strcmp(yrite, paattui))
      r = osio_loppui;
    else if(!strcmp(yrite, nimi)) {
      fscanf(f, "%s", yrite);
      if(!strcmp(yrite, "="))
	r = lue_cfg1(f, muuttuja);
    }
  }
  free(paattui);
  free(yrite);
  fclose(f);
  return r;
}
