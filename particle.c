#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

struct ControllerState {
	int a;
	float leftx;
	float lefty;
} controller_state;

//in pixels per second
#define WALKING_SPEED 128.0
#define PARTICLE_COUNT 1000
#define BASE_LIFE 1000
#define BASE_SIZE 16.0
struct Particle {
	float x;
	float y;
	float vx;
	float vy;
	//Time(ms) until destruction
	int life;
} particle;

void
quit(int exit_code) {
	SDL_Quit();
	exit(exit_code);
}

void
init() {
	//Check SDL Version
	SDL_version compiled;
	SDL_version linked;
	SDL_VERSION(&compiled);
	SDL_GetVersion(&linked);
	if (compiled.major != linked.major) {
		fprintf(stderr, "SDL version mismatch! Found version %d, require version %d", linked.major, compiled.major);
		quit(EXIT_FAILURE);
	}

	
	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER)) {
		fprintf(stderr, "Initialisation Error: %s", SDL_GetError());
		quit(EXIT_FAILURE);
	}

	//Hints
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

	//Activate all existing controllers
	SDL_GameControllerEventState(SDL_ENABLE);
	SDL_GameControllerAddMappingsFromFile("gamecontrollerdb.txt");
	
}

int
draw1(struct Particle particle, SDL_Texture *tex, SDL_Renderer *renderer) {
	int life = particle.life;
	float size = life < 0.1*BASE_LIFE ? life*BASE_SIZE/(0.1*BASE_LIFE): BASE_SIZE;
	//filledCircleRGBA(renderer, particles[i].x, particles[i].y, size, 0, 0, 0, 255);
	SDL_RenderCopy(renderer, tex, NULL, &(SDL_Rect){particle.x - size, particle.y - size, size*2, size*2});
	return 1;
}

int
main(int argc, char *argv[]) {
	init();

	//Setup Rendering System
	SDL_Window* window;
	SDL_Renderer* renderer;
	if(SDL_CreateWindowAndRenderer(WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE, &window, &renderer)) {
		fprintf(stderr, "Window creation failed: %s", SDL_GetError());
		quit(EXIT_FAILURE);
	}

	//Load Initial Sprites
	SDL_Texture* particle_sprite = IMG_LoadTexture(renderer, "particle.png");
	SDL_Texture* back_sprite = IMG_LoadTexture(renderer, "back.png");
	SDL_SetTextureBlendMode(particle_sprite, SDL_BLENDMODE_ADD);
	SDL_SetTextureBlendMode(back_sprite, SDL_BLENDMODE_BLEND);

	//Initialise Objects
	particle = (struct Particle){16,16,0};
	struct Particle particles[PARTICLE_COUNT] = {};
	for(int i = 0; i < PARTICLE_COUNT; i++) {
		particles[i].life = rand()*BASE_LIFE*5.0 / RAND_MAX;
		particles[i].x = -16;
		particles[i].y = -16;
	}

	//GAME LOOP
	int t = SDL_GetTicks();
	//Time between frames in milliseconds
	int dt = 0;
	while(1) {
		SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0);
		SDL_RenderClear(renderer);
		SDL_Event event;
		while(SDL_PollEvent(&event)) {
			if(event.type == SDL_QUIT) {
				quit(0);
			}
		}
		dt = SDL_GetTicks() - t;
		t += dt;
		for(int i = 0; i < PARTICLE_COUNT; i++) {
			//Accelerate
			particles[i].vy += -0.1*dt;
			//This keeps the vx unit in pixels/second
			particles[i].x += (particles[i].vx*dt/1000.0);
			particles[i].y += (particles[i].vy*dt/1000.0);

			
			//Bring out your dead
			particles[i].life -= dt;
			if(particles[i].life <= 0){
				particles[i].life = BASE_LIFE + rand() * 500.0 / RAND_MAX;
				particles[i].x = WINDOW_WIDTH/2 - 8;
				particles[i].y = WINDOW_HEIGHT/2 - 8 + 64;
				float theta = rand() * 2*M_PI / RAND_MAX;
				float v = rand() * 30.0 / RAND_MAX;
				particles[i].vx = v * cos(theta);
				particles[i].vy = v * sin(theta);
			}

		}

		for(int i = 0; i < PARTICLE_COUNT; i++) {
			draw1(particles[i], back_sprite, renderer);
		}

		for(int i = 0; i < PARTICLE_COUNT; i++) {
			int life = particles[i].life;
			int r, g, b;
			r = life > BASE_LIFE? 255 : life*(255/BASE_LIFE);
			if(life < 0.25*BASE_LIFE) {
				r = life*255/(0.25*BASE_LIFE);
			} else {
				r = 255;
			}
			if(life < 0.25*BASE_LIFE) {
				g = 0;
			} else if(life < 0.75*BASE_LIFE) {
				g = -128 + life*512.0/BASE_LIFE;
			} else {
				g = 255;
			}
			if(life < 0.5*BASE_LIFE) {
				b = 0;
			} else if(life < BASE_LIFE) {
				b = -255 + life*512.0/BASE_LIFE;
			} else {
				b = 255;
			}
			float size = life < 0.1*BASE_LIFE ? life*BASE_SIZE/(0.1*BASE_LIFE): BASE_SIZE;
			//filledCircleRGBA(renderer, particles[i].x, particles[i].y, 4, r, g, b, 255);
			 SDL_SetTextureColorMod(particle_sprite, r, g, b);
			 SDL_RenderCopy(renderer, particle_sprite, NULL, &(SDL_Rect){particles[i].x - size/2, particles[i].y - size/2, size, size});
		}

		SDL_RenderPresent(renderer);
	}
	return EXIT_SUCCESS;
}
