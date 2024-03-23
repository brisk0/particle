all: particle

release: particle.c
	clang particle.c -oparticle -lSDL2 -lSDL2_image -lSDL2_gfx -lm -lz -Wall -O2

particle: particle.c
	clang particle.c -oparticle -lSDL2 -lSDL2_image -lSDL2_gfx -lm -lz -Wall -O0 -g

run: particle
	./particle

.PHONY: all run
