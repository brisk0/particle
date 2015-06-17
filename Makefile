all: particle

release: particle.c gamecontrollerdb
	clang particle.c -oparticle -lSDL2 -lSDL2_image -lSDL2_gfx -lm -lz -Wall -O2

particle: particle.c
	clang particle.c -oparticle -lSDL2 -lSDL2_image -lSDL2_gfx -lm -lz -Wall -O0 -g

run: particle
	./particle

gamecontrollerdb:
	wget https://raw.githubusercontent.com/gabomdq/SDL_GameControllerDB/master/gamecontrollerdb.txt

.PHONY: all run
