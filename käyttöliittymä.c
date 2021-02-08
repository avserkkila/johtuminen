#include <stdio.h>
#include "askellus.h"
#include "cfgmuuttujat.h"
#include "käyttöliittymä.h"
#include "lue_cfg.h"

char seur_askel();
unsigned askel_n = 0;

char tasovalinta(gtyyp** grd, short k) {
  if(k<0 || k>=resolz)
    return 1;
  *grd = grid.g + gridind(0,0,k);
  return 0;
}

gtyyp* anna_taso(short k) {
  if(k<0 || k>=resolz)
    return NULL;
  if(ndim == 3)
    return grid.g + gridind(0,0,k);
  return grid.g;
}

int kaunnista(struct kuvamaareet *kuva) {
  kuva->ztaso = (resolz-1) / 2;
  kuva->piirto_x0 = 0;
  kuva->piirto_y0 = 0;
  int piirtox0_ennen = 0, piirtoy0_ennen = 0;
  gtyyp* tallgrd = grid.g+gridind(0,0,tallkork);
  gtyyp* grd = grid.g;
  tasovalinta(&grd, kuva->ztaso);
  int jaljella_piirtamiseen = piirtovali;
  char kontrol = 0;
  char koko_lukittu = 0;
  char ruutu_klikattu = 0;
  char ruutua_raahattiin = 0;
  int xklik, yklik;
  SDL_Event tapaht;
  if(tallenna) {
    ftallenna_grid(kuva->f, kuva->varikerr, tallgrd); //tallennetaan alkutilanne
    goto TALLENNA;
  }
  while(1) {
    while(SDL_PollEvent(&tapaht)) {
      switch(tapaht.type) {
      case SDL_QUIT:
	return 0;
      case SDL_KEYDOWN:
	switch(tapaht.key.keysym.sym)
	  {
	  case SDLK_SPACE: //tauolle tai pois sieltä
	    if(piirtovali)
	      piirtovali = 0;
	    else
	      lue_cfgmuuttuja("ajotapa", "piirtovali", &piirtovali);
	    break;
	  case SDLK_KP_1:
	  case SDLK_1: //ota yksi aika-aksel
	    if(seur_askel())
	      return 0;
	    tasovalinta(&grd, kuva->ztaso);
	    piirra(grd, kuva);
	    break;
	  case SDLK_KP_PLUS:
	  case SDLK_PLUS:
	    kuva->kokokerr.x *= 1.1;
	    kuva->kokokerr.y *= 1.1;
	    break;
	  case SDLK_KP_MINUS:
	  case SDLK_MINUS:
	    kuva->kokokerr.x *= 1/1.1;
	    kuva->kokokerr.y *= 1/1.1;
	    break;
	  case SDLK_UP:
	    if(tasovalinta(&grd, ++(kuva->ztaso))) {
	      puts("\nHähää, eipäs pääse ylöspäin");
	      --(kuva->ztaso);
	    }
	    break;
	  case SDLK_DOWN:
	    if(tasovalinta(&grd, --(kuva->ztaso))) {
	      puts("\nHähää, eipäs pääse alaspäin");
	      ++(kuva->ztaso);
	    }
	    break;
	  case SDLK_RETURN:
	    free(Treun);
	    cfgmuuttujat("ajotapa grid lämpötilat");
	    free(grid.reuna);
	    puts("");
	    grid_jako();
	    kuvan_kertoimet(kuva);
	    grid_Treun(grid.g);
	    for(int i=0; i<gridpit; i++)
	      grid.g2[i] = grid.g[i];
	    break;
	  case SDLK_LCTRL:
	  case SDLK_RCTRL:
	    kontrol = 1;
	    break;
	  case SDLK_l:
	    if(!kontrol)
	      break;
	    if(koko_lukittu) {
	      koko_lukittu = 0;
	      kuvan_kertoimet(kuva);
	    } else {
	      koko_lukittu = 1;
	    }
	    kontrol = 0;
	    break;
	  }
	break;
      case SDL_KEYUP:
	switch(tapaht.key.keysym.sym)
	  {
	  case SDLK_LCTRL:
	  case SDLK_RCTRL:
	    kontrol = 0;
	    break;
	  }
	break;
      case SDL_MOUSEBUTTONDOWN:;
	xklik = tapaht.motion.x / kuva->kokokerr.x;
	yklik = tapaht.motion.y / kuva->kokokerr.y;
	if(xklik-kuva->piirto_x0 < resolx && xklik-kuva->piirto_x0 >= 0 &&
	   yklik-kuva->piirto_y0 < resoly && yklik-kuva->piirto_y0 >= 0) {
	  ruutu_klikattu = 1;
	  piirtox0_ennen = kuva->piirto_x0;
	  piirtoy0_ennen = kuva->piirto_y0;
	}
	break;
      case SDL_MOUSEBUTTONUP:
	if(ruutu_klikattu)
	  if(!ruutua_raahattiin)
	    printf("\nT(%i, %i, %i) = %g\n",
		   xklik-kuva->piirto_x0, yklik-kuva->piirto_y0, kuva->ztaso,
		   grd[gridind(xklik-kuva->piirto_x0,yklik-kuva->piirto_y0,kuva->ztaso)]);
	ruutua_raahattiin = 0;
	ruutu_klikattu = 0;
	break;
      case SDL_MOUSEMOTION:
	if(ruutu_klikattu) {
	  kuva->piirto_x0 = piirtox0_ennen + tapaht.motion.x / kuva->kokokerr.x - xklik;
	  kuva->piirto_y0 = piirtoy0_ennen + tapaht.motion.y / kuva->kokokerr.y - yklik;
	  ruutua_raahattiin = 1;
	}
      case SDL_WINDOWEVENT:
	switch(tapaht.window.event) {
	case SDL_WINDOWEVENT_RESIZED:
	  ikkuna_w = tapaht.window.data1;
	  ikkuna_h = tapaht.window.data2;
	  if(!koko_lukittu)
	    kuvan_kertoimet(kuva);
	  break;
	}
	
      } //switch tapaht.type
    } //while(SDL_PollEvent(&tapaht))
    /*päivitetään aika-askel*/
    if (tallenna) {
    TALLENNA:
      for(int i=0; i<tallvali; i++)
	if(seur_askel())
	  return 0;

      tasovalinta(&tallgrd, tallkork); //kohdistaa grd:n uudestaan grid.g:hen
      ftallenna_grid(kuva->f, kuva->varikerr, tallgrd);
      if(piirto_heti) {
	if((jaljella_piirtamiseen -= tallvali) <= 0) {
	  tasovalinta(&grd, kuva->ztaso); //kohdistaa grd:n uudestaan grid.g:hen
	  piirra(grd, kuva);
	  jaljella_piirtamiseen = piirtovali;
	}
      } else {
	goto TALLENNA;
      }
    } else {
      for(int i=0; i<piirtovali; i++)
	if(seur_askel())
	  return 0;
      tasovalinta(&grd, kuva->ztaso);
      piirra(grd, kuva);
    }
    SDL_Delay(piirtoviive);
  }
  return 0;
}

char seur_askel() {
  gtyyp* apu = grid.g;
  grid.g = askel(grid.g, grid.g2);
  grid.g2 = apu;
  ++askel_n;
  printf("\r%i / %i", askel_n, askel_max);
  ohjelmoitava_muutos();
  if(askel_n == askel_max)
    return 1;
  return 0;
}

#define xypit (resolx*resoly)

void katso(struct kuvamaareet* k) {
  int nkuva = 0;
  char kontrol = 0;
  char koko_lukittu = 0;
  k->piirto_x0 = 0;
  k->piirto_y0 = 0;
  int piirtox0_ennen = 0, piirtoy0_ennen = 0;
  char ruutu_klikattu = 0;
  char ruutua_raahattiin = 0;
  int xklik, yklik;
  k->ztaso = (resolz-1)/2;
  fseek(k->f, k->ztaso*xypit, SEEK_CUR);
  if(k->g) {
    fseek(k->g, 4*sizeof(int)+2*sizeof(gtyyp), SEEK_SET);
    fseek(k->g, k->ztaso*xypit, SEEK_CUR);
  }
  fpiirra(k);
  SDL_Event tapaht;
  while(1) {
    while(SDL_PollEvent(&tapaht))
      switch(tapaht.type) {
      case SDL_QUIT:
	return;
      case SDL_KEYDOWN:
	switch(tapaht.key.keysym.sym)
	  {
	  case SDLK_SPACE: //tauolle tai pois sieltä
	    if(piirtovali)
	      piirtovali = 0;
	    else
	      lue_cfgmuuttuja("ajotapa", "piirtovali", &piirtovali);
	    break;
	  case SDLK_KP_1:
	  case SDLK_1:
	  case SDLK_RIGHT: //seuraava kuva
	    fseek(k->f, xypit*(resolz-1), SEEK_CUR);
	    if(k->g) fseek(k->g, xypit*(resolz-1), SEEK_CUR);
	    nkuva++;
	    if(fpiirra(k) == EOF)
	      nkuva--;
	    break;
	  case SDLK_LEFT: //edellinen kuva
	    if(nkuva) {
	      fseek(k->f, -xypit*(resolz+1), SEEK_CUR);
	      if(k->g) fseek(k->g, -xypit*(resolz+1), SEEK_CUR);
	      nkuva--;
	      fpiirra(k);
	    }
	    break;
	  case SDLK_UP:
	    if(++(k->ztaso) >= resolz) {
	      puts("\nHähää, eipäs pääse ylöspäin");
	      --(k->ztaso);
	    } else {
	      fpiirra(k);
	    }
	    break;
	  case SDLK_DOWN:
	    if(--(k->ztaso) < 0) {
	      puts("\nHähää, eipäs pääse alaspäin");
	      ++(k->ztaso);
	    } else {
	      fseek(k->f, -2*xypit, SEEK_CUR);
	      if(k->g) fseek(k->g, -2*xypit, SEEK_CUR);
	      fpiirra(k);
	    }
	    break;
	  case SDLK_KP_PLUS:
	  case SDLK_PLUS:
	    k->kokokerr.x *= 1.1;
	    k->kokokerr.y *= 1.1;
	    fseek(k->f, -xypit, SEEK_CUR);
	    if(k->g) fseek(k->g, -xypit*(resolz+1), SEEK_CUR);
	    fpiirra(k);
	    break;
	  case SDLK_KP_MINUS:
	  case SDLK_MINUS:
	    k->kokokerr.x *= 1/1.1;
	    k->kokokerr.y *= 1/1.1;
	    fseek(k->f, -xypit, SEEK_CUR);
	    if(k->g) fseek(k->g, -xypit*(resolz+1), SEEK_CUR);
	    fpiirra(k);
	    break;
	  case SDLK_HOME:
	  case SDLK_RETURN:
	    fseek(k->f, 4*sizeof(int)+2*sizeof(gtyyp), SEEK_SET);
	    fseek(k->f, k->ztaso*xypit, SEEK_CUR);
	    if(k->g) {
	      fseek(k->g, 4*sizeof(int)+2*sizeof(gtyyp), SEEK_SET);
	      fseek(k->g, k->ztaso*xypit, SEEK_CUR);
	    }
	    nkuva = 0;
	    fpiirra(k);
	    break;
	  case SDLK_END:
	    fseek(k->f, -xypit*(resolz-k->ztaso), SEEK_END);
	    if(k->g) fseek(k->g, -xypit*(resolz-k->ztaso), SEEK_END);
	    fpiirra(k);
	    break;
	  case SDLK_LCTRL:
	  case SDLK_RCTRL:
	    kontrol = 1;
	    break;
	  case SDLK_l:
	    if(!kontrol)
	      break;
	    if(koko_lukittu) {
	      koko_lukittu = 0;
	      kuvan_kertoimet(k);
	    } else {
	      koko_lukittu = 1;
	    }
	    kontrol = 0;
	    break;
	  }
	break;
      case SDL_KEYUP:
	switch(tapaht.key.keysym.sym)
	  {
	  case SDLK_LCTRL:
	  case SDLK_RCTRL:
	    kontrol = 0;
	    break;
	  }
	break;
      case SDL_MOUSEBUTTONDOWN:
	xklik = tapaht.motion.x/k->kokokerr.x;
	yklik = tapaht.motion.y/k->kokokerr.y;
	if(xklik-k->piirto_x0 < resolx && xklik-k->piirto_x0 >= 0 &&
	   yklik-k->piirto_y0 < resoly && yklik-k->piirto_y0 >= 0) {
	  ruutu_klikattu = 1;
	  piirtox0_ennen = k->piirto_x0;
	  piirtoy0_ennen = k->piirto_y0;
	}
	break;
      case SDL_MOUSEBUTTONUP:
	if(ruutu_klikattu) {
	  if(!ruutua_raahattiin) {
	    fseek(k->f, -xypit, SEEK_CUR); //kuva tai taso taaksepäin
	    fseek(k->f, yklik+resoly*xklik, SEEK_CUR); //kys. ruudun kohdalle
	    float T = fgetc(k->f)/k->varikerr + Tmin; //laske lämpötila
	    printf("T(%i, %i, %i) = %f\n",
		   xklik-k->piirto_x0, yklik-k->piirto_y0, k->ztaso, T);
	    fseek(k->f, xypit-(yklik+resoly*xklik), SEEK_CUR); //takaisin
	  }
	  ruutu_klikattu = 0;
	}
	ruutua_raahattiin = 0;
      case SDL_MOUSEMOTION:
	if(ruutu_klikattu) {
	  k->piirto_x0 = piirtox0_ennen + tapaht.motion.x / k->kokokerr.x - xklik;
	  k->piirto_y0 = piirtoy0_ennen + tapaht.motion.y / k->kokokerr.y - yklik;
	  ruutua_raahattiin = 1;
	}
      case SDL_WINDOWEVENT:
	switch(tapaht.window.event) {
	case SDL_WINDOWEVENT_RESIZED:
	  ikkuna_w = tapaht.window.data1;
	  ikkuna_h = tapaht.window.data2;
	  if(!koko_lukittu)
	    kuvan_kertoimet(k);
	  fseek(k->f, -xypit, SEEK_CUR);
	  if(k->g) fseek(k->g, -xypit, SEEK_CUR);
	  fpiirra(k);
	  break;
	}
      }
    if(piirtovali) {
      fseek(k->f, (piirtovali-1)*resolx*resoly*resolz + resolx*resoly*(resolz-1), SEEK_CUR);
      if(k->g)
	fseek(k->g, (piirtovali-1)*resolx*resoly*resolz + resolx*resoly*(resolz-1), SEEK_CUR);
      nkuva += piirtovali;
      if(fpiirra(k) == EOF) {
	piirtovali = 0;
	puts("valmis");
      }
    }
    SDL_Delay(piirtoviive);
  }
}
