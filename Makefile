SRC_TIED=main.c cfgmuuttujat.c lue_cfg.c ylista.c grid.c piirrä.c askellus.c käyttöliittymä.c ohjelmoitava_muutos.c

all: kaanna aja

kaanna: 
	gcc -g3 -Wall -Wno-unused-result -o lämpö ${SRC_TIED} -lm -lSDL2 -O0

aja:
	./lämpö

kaanna3:
	gcc -g3 -Wall -Wno-unused-result -o lämpö ${SRC_TIED} -lm -lSDL2 -O3
