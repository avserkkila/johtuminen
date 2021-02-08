#include <stdlib.h>
#include <stdio.h>
#include "askellus.h"
#include "cfgmuuttujat.h"
#define ind2(x,y) ((y) + resoly*(x))
#define t2(x,y) grd[ind2(x,y)]
#define ind3(x,y,z) ((y) + resoly*(x) + resolx*resoly*(z))
#define t3(x,y,z) grd[(y) + resoly*(x) + resolx*resoly*(z)]

gtyyp* askel(gtyyp* grd, gtyyp* kohde) {
  /*tässä ei muuteta reunoja*/
  switch (ndim)
    {
    case 1:
      return askel_1d(grd, kohde);
    case 2:
      return askel_2d(grd, kohde);
    case 3:
      return askel_3d(grd, kohde);
    default:
      fprintf(stderr, "Virhe: %i ulottuvuutta, pitäisi olla 1 – 3\n", ndim);
      return NULL;
    }
}

gtyyp* askel_1d(gtyyp* restrict grd, gtyyp* restrict kohde) {
  gtyyp jakaja = 2 + Tkkeski;
  for(int i=1; i<resoly-1; i++)
    kohde[i] = (grd[i]*Tkkeski + grd[i-1] + grd[i+1]) / jakaja;
  return kohde;
}

gtyyp* askel_2d(gtyyp* restrict grd, gtyyp* restrict kohde) {
  int i=1;
  //i = 1
  kohde[ind2(1,1)] = (t2(1,1)*Tkkeski +					\
		      t2(1,0)*Tkreun[grid.lohko[1]] +			\
		      t2(0,1)*Tkreun[grid.okhol[1]] +			\
		      t2(1,2) + t2(2,1)) /				\
    (2 + Tkkeski + Tkreun[grid.lohko[1]] + Tkreun[grid.okhol[1]]);
  int j=2;
  for(j=2; j<resoly-2; j++) {
    kohde[ind2(i,j)] = (t2(i,j)*Tkkeski +			\
			t2(i,j-1) + t2(i,j+1) + t2(i+1,j) +	\
			t2(i-1,j)*Tkreun[grid.okhol[j]]) /	\
      (3 + Tkkeski + Tkreun[grid.okhol[j]]);
  }
  kohde[ind2(1,j)] = (t2(1,j)*Tkkeski +				\
		      t2(0,j)*Tkreun[grid.okhol[j]] +		\
		      t2(1,j+1)*Tkreun[grid.okhol[j+2]] +	\
		      t2(1,j-1) + t2(2,j)) /			\
    (2 + Tkkeski + Tkreun[grid.okhol[j]] + Tkreun[grid.okhol[j+2]]);

  //i = keskialue
  for(i=2; i<resolx-2; i++) {
    kohde[ind2(i,1)] = (t2(i,1)*Tkkeski +				\
			t2(i,0)*Tkreun[grid.lohko[i]] +		\
			t2(i,2) + t2(i-1,1) + t2(i+1,1)) /	\
      (3 + Tkkeski + Tkreun[grid.lohko[i]]);
    for(j=2; j<resoly-2; j++) {
      kohde[ind2(i,j)] = (t2(i,j)*Tkkeski +				\
			  t2(i,j-1) + t2(i,j+1) + t2(i-1,j) + t2(i+1,j)) / \
	(4 + Tkkeski);
    }
    kohde[ind2(i,j)] = (t2(i,j)*Tkkeski +				\
			t2(i,j+1)*Tkreun[grid.okhol[resoly+i]] +	\
			t2(i,j-1) + t2(i-1,j) + t2(i+1,j)) /	\
      (3 + Tkkeski + Tkreun[grid.okhol[j+1+i]]);
  }
  
  //i = oikea reuna
  kohde[ind2(i,1)] = (t2(i,1)*Tkkeski +\
		      t2(i,0)*Tkreun[grid.lohko[i]] +\
		      t2(i+1,1)*Tkreun[grid.lohko[i+2]] +\
		      t2(i,2) + t2(i-1,1)) /\
    (2 + Tkkeski + Tkreun[grid.lohko[i]] + Tkreun[grid.lohko[i+2]]);
  for(j=2; j<resoly-2; j++) {
    kohde[ind2(i,j)] = (t2(i,j)*Tkkeski +\
			t2(i+1,j)*Tkreun[grid.lohko[i+1+j]] +\
			t2(i-1,j) + t2(i,j-1) + t2(i,j+1)) /\
      (3 + Tkkeski + Tkreun[grid.lohko[i+1+j]]);
  }
  kohde[ind2(i,j)] = (t2(i,j)*Tkkeski +					\
		      t2(i+1,j)*Tkreun[grid.lohko[i+j+1]] +		\
		      t2(i,j+1)*Tkreun[grid.lohko[i+j+3]] +		\
		      t2(i,j-1) + t2(i-1,j)) /				\
    (2 + Tkkeski + Tkreun[grid.lohko[i+j+1]] + Tkreun[grid.lohko[i+j+3]]);
  return kohde;
}

#define nurkka3(i,j,k,iu,ju,ku,is,js,ks,ilhko,jlhko,klhko) ({kohde[ind3(i,j,k)] = \
	(t3(i,j,k)*Tkkeski + t3(iu,j,k)*ilhko + t3(i,ju,k)*jlhko + t3(i,j,ku)*klhko + \
	 t3(is,j,k) + t3(i,js,k) + t3(i,j,ks)) /			\
	(3 + Tkkeski + ilhko + jlhko + klhko); })

#define jnurna3(i,j,k,iu,ku,is,ks,ilhko,klhko) ({kohde[ind3(i,j,k)] =	\
	(t3(i,j,k)*Tkkeski + t3(iu,j,k)*ilhko + t3(i,j,ku)*klhko +	\
	 t3(is,j,k) + t3(i,j,ks) + t3(i,j-1,k) + t3(i,j+1,k)) /		\
	(4 + Tkkeski + ilhko + klhko); })

#define inurna3(i,j,k,ju,ku,js,ks,jlhko,klhko) ({kohde[ind3(i,j,k)] =	\
	(t3(i,j,k)*Tkkeski + t3(i,ju,k)*jlhko + t3(i,j,ku)*klhko +	\
	 t3(i,js,k) + t3(i,j,ks) + t3(i-1,j,k) + t3(i+1,j,k)) /		\
	(4 + Tkkeski + jlhko + klhko); })

#define knurna3(i,j,k,iu,ju,is,js,ilhko,jlhko) ({kohde[ind3(i,j,k)] =	\
	(t3(i,j,k)*Tkkeski + t3(iu,j,k)*ilhko + t3(i,ju,k)*jlhko +	\
	 t3(is,j,k) + t3(i,js,k) + t3(i,j,k-1) + t3(i,j,k+1)) /		\
	(4 + Tkkeski + ilhko + jlhko); })

#define kreuna3(i,j,k,ku,ks,klhko) ({kohde[ind3(i,j,k)] =		\
	(t3(i,j,k)*Tkkeski + t3(i,j,ku)*klhko + t3(i,j,ks) +		\
	 t3(i-1,j,k) + t3(i+1,j,k) + t3(i,j-1,k) + t3(i,j+1,k)) /	\
	(5 + Tkkeski + klhko); })

#define ireuna3(i,j,k,iu,is,ilhko) ({kohde[ind3(i,j,k)] =\
	(t3(i,j,k)*Tkkeski + t3(iu,j,k)*ilhko + t3(is,j,k) +\
	 t3(i,j,k-1) + t3(i,j,k+1) + t3(i,j-1,k) + t3(i,j+1,k)) /\
	(5 + Tkkeski + ilhko); })

#define jreuna3(i,j,k,ju,js,jlhko) ({kohde[ind3(i,j,k)] =		\
	(t3(i,j,k)*Tkkeski + t3(i,ju,k)*jlhko + t3(i,js,k) +		\
	 t3(i-1,j,k) + t3(i+1,j,k) + t3(i,j,k-1) + t3(i,j,k+1)) /	\
	(5 + Tkkeski + jlhko); })

#define sisa3(i,j,k) ({kohde[ind3(i,j,k)] =				\
	(t3(i,j,k)*Tkkeski + t3(i-1,j,k) + t3(i+1,j,k) +		\
	 t3(i,j,k-1) + t3(i,j,k+1) + t3(i,j-1,k) + t3(i,j+1,k)) /	\
	(6 + Tkkeski); })

gtyyp* askel_3d(gtyyp* restrict grd, gtyyp* restrict kohde) {
  //z = 1
  int k,j,i;
  
  /*ensimmäinen j-palkki alatasolla*/
  nurkka3(1,1,1, 0,0,0, 2,2,2, Tkreun[grid.okhol[1]],Tkreun[grid.lohko[1]],Tkala);
  for(j=2; j<resoly-2; j++)
    jnurna3(1,j,1, 0,0, 2,2, Tkreun[grid.okhol[j]],Tkala);
  nurkka3(1,j,1, 0,j+1,0, 2,j-1,2, Tkreun[grid.okhol[j]],Tkreun[grid.okhol[j+2]],Tkala);

  /*alimman tason keskiosa*/
  for(i=2; i<resolx-2; i++) {
    inurna3(i,1,1, 0,0, 2,2, Tkreun[grid.lohko[i]], Tkala);
    for(j=2; j<resoly-2; j++)
      kreuna3(i,j,1,0,2,Tkala);
    inurna3(i,j,1 ,j+1,0 ,j-1,2, Tkreun[grid.okhol[i+j+1]], Tkala);
  }

  /*viimeinen j-palkki alatasolla*/
  nurkka3(i,1,1, i+1,0,0, i-1,2,2, Tkreun[grid.lohko[i+2]],Tkreun[grid.lohko[i]],Tkala);
  for(j=2; j<resoly-2; j++)
    jnurna3(i,j,1, i+1,0, i-1,2, Tkreun[grid.lohko[i+j+1]],Tkala);
  nurkka3(i,j,1, i+1,j+1,0, i-1,j-1,2,					\
	  Tkreun[grid.lohko[i+j+1]], Tkreun[grid.lohko[i+j+3]], Tkala);

  /*välitasot*/
  for(k=2; k<resolz-2; k++) {
    /*ensimmäinen j-palkki välitasolla*/
    knurna3(1,1,k, 0,0, 2,2, Tkreun[grid.lohko[1]], Tkreun[grid.okhol[1]]);
    for(j=2; j<resoly-2; j++)
      ireuna3(1,j,k, 0,2, Tkreun[grid.okhol[j]]);
    knurna3(1,j,k, 0,j+1, 2,j-1, Tkreun[grid.okhol[i+j+1]], Tkreun[grid.okhol[j]]);

    /*sisäosat*/
    for(i=2; i<resolx-2; i++) {
      jreuna3(i,1,k, 0,2, Tkreun[grid.lohko[i]]);
      for(j=2; j<resoly-2; j++)
	sisa3(i,j,k);
      jreuna3(i,j,k, j+1,j-1, Tkreun[grid.okhol[i+j+1]]);
    }

    /*viimeinen j-palkki välitasolla*/
    knurna3(i,1,k, i+1,0, i-1,2, Tkreun[grid.lohko[i+2]], Tkreun[grid.lohko[i]]);
    for(j=2; j<resoly-2; j++)
      ireuna3(i,j,k, i+1,i-1, Tkreun[grid.lohko[i+j+1]]);
    knurna3(i,j,k, i+1,j+1, i-1,j-1, Tkreun[grid.lohko[i+j+1]], Tkreun[grid.lohko[i+j+3]]);
  }

  /*ensimmäinen j-palkki ylätasolla*/
  nurkka3(1,1,k, 0,0,k+1, 2,2,k-1, Tkreun[grid.okhol[1]], Tkreun[grid.lohko[1]], Tkula);
  for(j=2; j<resoly-2; j++)
    jnurna3(1,j,k, 0,k+1, 2,k-1, Tkreun[grid.okhol[j]], Tkula);
  nurkka3(1,j,k, 0,j+1,k+1, 2,j-1,k-1, Tkreun[grid.okhol[j]], Tkreun[grid.okhol[j+2]], Tkula);

  /*ylimmän tason keskiosa*/
  for(i=2; i<resolx-2; i++) {
    inurna3(i,1,k, 0,k+1, 2,k-1, Tkreun[grid.lohko[i]], Tkula);
    for(j=2; j<resoly-2; j++)
      kreuna3(i,j,k, k+1,k-1, Tkula);
    inurna3(i,j,k, j+1,k+1, j-1,k-1, Tkreun[grid.okhol[i+j+i]], Tkula);
  }

  /*viimeinen j-palkki ylätasolla*/
  nurkka3(i,1,k, i+1,0,k+1, i-1,2,k-1, Tkreun[grid.lohko[i+j+1]], Tkreun[grid.lohko[i]], Tkula);
  for(j=2; j<resoly-2; j++)
    jnurna3(i,j,k, i+1,k+1, i-1,k-1, Tkreun[grid.lohko[i+j+1]], Tkula);
  nurkka3(i,j,k, i+1,j+1,k+1, i-1,j-1,k-1, Tkreun[grid.lohko[i+j+1]], Tkreun[grid.lohko[i+j+3]], Tkula);
    
  return kohde;
}


#if 0
gtyyp* askel_2d_vanha(gtyyp* restrict grd, gtyyp* restrict kohde) {
  for(int j=2; j<resoly-2; j++)
    for(int i=2; i<resolx-2; i++)
      kohde[j][i] =
	t(i,j) - ( Tkkeski*t(i,j) + t(i+1,j)+t(i-1,j)-t(i,j-1)-t(i,j+1) ) * (4+Tkkeski!)
  
  /*lienisi hidasta mennä askel kerallaan ja tarkistaa aina,
    onko paikka jakoraja, asetetaan sen sijaan yläraja ehtolauseella*/
  /*nurkat tehdään nyt kahdesti, jos ne ovat jakorajoina
    ei pitäisi aiheuttaa virhettä, mutta hidastaa vähän*/
  short ularaja;
  char jakorajalla;
  int umparus = resolx*2+resoly*2-4;
  for(int r=0; r<nens; r++) {
    short i,j;
    i=grid.jakox[r];
    j=grid.jakoy[r];
    switch(grid.reuna[r])
      {
      case 1:
      CASE1:
	if(i==0) { //vasen ylänurkka
	  float Tkvasen = Tkreun[grid.lohko[umparus]];
	  float Tkula   = Tkreun[grid.lohko[1]];
	  kohde(1,1) = t(1,1) + (
	    -Tkvasen * (t(1,1)-t(0,1))
	    -Tkula   * (t(1,1)-t(1,0))
	    -Tkerr   * (2*t(1,1)-t(2,1)-t(1,2)) ) / (Tkerr*2+Tkvasen+Tkula);
	  i = 2;
	}
	if(grid.reuna[r+1] == 1) { //jakoraja tällä laidalla
	  ularaja = grid.jakox[r+1];
	  jakorajalla = 1;
	} else {                   //jakoraja eri laidalla
	  ularaja = resolx-2; //ei mennä nurkkaan asti
	  jakorajalla = 0;
	}
	for(; i<ularaja; i++)
	  kohde(i,1) = t(i,1) + (
	    -Tkreun[r] * (t(i,1)-t(i,0))
	    -Tkerr *(3*t(i,1)-t(i,2)-t(i-1,1)-t(i+1,1)) ) / (3*Tkerr+Tkreun[r]);
	if(jakorajalla) continue;
      case 2:
	i=resolx-2;
	if(j==0) {
	  float Tkula   = Tkreun[grid.lohko[resolx-2]];
	  float Tkoikea = Tkreun[grid.lohko[resolx]];
	  kohde(i,1) = t(i,1) + (
	    -Tkula   * (t(i,1)-t(i,0))
	    -Tkoikea * (t(i,1)-t(i+1,1))
	    -Tkerr   * (2*t(i,1)-t(i-1,1)-t(i,2)) ) / (Tkerr*2+Tkula+Tkoikea);
	  j = 2;
	}
	if(grid.reuna[r+1] == 2) {
	  ularaja = grid.jakoy[r+1];
	  jakorajalla = 1;
	} else {
	  ularaja = resoly-2;
	  jakorajalla = 0;
	}
	for(; j<ularaja; j++)
	  kohde(i,j) = t(i,j) + (
	    -Tkreun[r] * (t(i,j)-t(i+1,j))
	    -Tkerr *(3*t(i,j)-t(i,j-1)-t(i,j+1)-t(i-1,j)) ) / (3*Tkerr+Tkreun[r]);
	if(jakorajalla) continue;
      case 3:
	j=resoly-2;
	if(i==resolx-1) {
	  i--;
	  float Tkoikea = Tkreun[grid.lohko[umparus/2-1]];
	  float Tkala   = Tkreun[grid.lohko[umparus/2+1]];
	  kohde(i,j) = t(i,j) + (
	    -Tkoikea * (t(i,j)-t(i+1,j))
	    -Tkala   * (t(i,j)-t(i,j+1))
	    -Tkerr   * (2*t(i,j)-t(i-1,j)-t(i,j-1)) ) / (Tkerr*2+Tkoikea+Tkala);
	  i--;
	}
	if(grid.reuna[r+1] == 3) {
	  ularaja = grid.jakox[r+1];
	  jakorajalla = 1;
	} else {
	  ularaja = 1;
	  jakorajalla = 0;
	}
	for(; i>ularaja; i--)
	  kohde(i,j) = t(i,j) + (
	    -Tkreun[r] * (t(i,j)-t(i,j+1))
	    -Tkerr *(3*t(i,j)-t(i,j-1)-t(i+1,j)-t(i-1,j)) ) / (3*Tkerr+Tkreun[r]);
	if(jakorajalla) continue;
      case 4:
	if(j==resoly-1) {
	  j--;
	  float Tkvasen = Tkreun[grid.lohko[umparus/2+resolx]];
	  float Tkala   = Tkreun[grid.lohko[umparus+1-resoly]];
	  kohde(1,j) = t(1,j) + (
	    -Tkvasen * (t(1,j)-t(0,j))
	    -Tkala   * (t(1,j)-t(1,j+1))
	    -Tkerr   * (2*t(1,j)-t(2,j)-t(1,j-1)) ) / (2*Tkerr+Tkvasen+Tkala);
	  j--;
	}
	if(grid.reuna[r+1] == 4) {
	  ularaja = grid.jakoy[r+1];
	  jakorajalla = 1;
	} else {
	  ularaja = 1;
	  jakorajalla = 0;
	}
	for(; j>ularaja; j--)
	  kohde(1,j) = t(1,j) + (
	    -Tkreun[r] * (t(1,j)-t(0,j))
	    -Tkerr *(3*t(1,j)-t(2,j)-t(1,j+1)-t(1,j-1)) ) / (3*Tkerr+Tkreun[r]);
	if(jakorajalla) continue;
	goto CASE1;
      }
  }
  return kohde;
}

gtyyp*** askel_3d(gtyyp*** restrict grd, gtyyp*** restrict kohde) {
  for(int k=1; k<resolz-1; k++)
    for(int j=1; j<resoly-1; j++)
      for(int i=1; i<resolx-1; i++)
	kohde[k][j][i] = u(i,j,k) - Tkerr*( 6*u(i,j,k)
					    -u(i-1, j   , k  )
					    -u(i+1, j   , k  )
					    -u(i  , j-1 , k  )
					    -u(i  , j+1 , k  )
					    -u(i  , j   , k-1)
					    -u(i  , j   , k+1) );
  return kohde;
}
#endif
