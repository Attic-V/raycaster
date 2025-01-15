#include <stdbool.h>

#include <SDL2/SDL.h>

#include "map.h"
#include "player.h"
#include "window.h"

#define PI 3.1415926535897932384626433

void move (void);
void render (SDL_Renderer *renderer);

Player player;

typedef enum {
	KEY_w = (1 << 0),
	KEY_a = (1 << 1),
	KEY_s = (1 << 2),
	KEY_d = (1 << 3),
	KEY_COMMA = (1 << 4),
	KEY_o = (1 << 5),
	KEY_e = (1 << 6),
	KEY_UP = (1 << 7),
	KEY_LEFT = (1 << 8),
	KEY_DOWN = (1 << 9),
	KEY_RIGHT = (1 << 10),
} Keys;

int keypresses = 0;

int main (void)
{
	SDL_Init(SDL_INIT_VIDEO);

	SDL_Window *window = SDL_CreateWindow(
		"raycaster",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		WINDOW_WIDTH, WINDOW_HEIGHT,
		SDL_WINDOW_BORDERLESS
	);

	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);

	player = (Player){
		.x = 1.5,
		.y = 1.5,
		.dir = 0,
	};

	bool quit = false;
	while (!quit) {
		SDL_Event event;
		SDL_PollEvent(&event);
		quit =
			(event.type == SDL_QUIT) ||
			(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
		;
		if (event.type == SDL_KEYDOWN) {
			switch (event.key.keysym.sym) {
				case SDLK_w: keypresses |= KEY_w; break;
				case SDLK_a: keypresses |= KEY_a; break;
				case SDLK_s: keypresses |= KEY_s; break;
				case SDLK_d: keypresses |= KEY_d; break;
				case SDLK_COMMA: keypresses |= KEY_COMMA; break;
				case SDLK_o: keypresses |= KEY_o; break;
				case SDLK_e: keypresses |= KEY_e; break;
				case SDLK_UP: keypresses |= KEY_UP; break;
				case SDLK_LEFT: keypresses |= KEY_LEFT; break;
				case SDLK_DOWN: keypresses |= KEY_DOWN; break;
				case SDLK_RIGHT: keypresses |= KEY_RIGHT; break;
			}
		}
		if (event.type == SDL_KEYUP) {
			switch (event.key.keysym.sym) {
				case SDLK_w: keypresses &= ~0 ^ KEY_w; break;
				case SDLK_a: keypresses &= ~0 ^ KEY_a; break;
				case SDLK_s: keypresses &= ~0 ^ KEY_s; break;
				case SDLK_d: keypresses &= ~0 ^ KEY_d; break;
				case SDLK_COMMA: keypresses &= ~0 ^ KEY_COMMA; break;
				case SDLK_o: keypresses &= ~0 ^ KEY_o; break;
				case SDLK_e: keypresses &= ~0 ^ KEY_e; break;
				case SDLK_UP: keypresses &= ~0 ^ KEY_UP; break;
				case SDLK_LEFT: keypresses &= ~0 ^ KEY_LEFT; break;
				case SDLK_DOWN: keypresses &= ~0 ^ KEY_DOWN; break;
				case SDLK_RIGHT: keypresses &= ~0 ^ KEY_RIGHT; break;
			}
		}

		move();

		render(renderer);
	}

	SDL_DestroyWindow(window);

	SDL_Quit();
}

void move (void)
{
	static const double movementspeed = 0.005;
	static const double turnspeed = 2 * PI * 0.003;
	if (keypresses & KEY_w | keypresses & KEY_COMMA | keypresses & KEY_UP) {
		player.x += movementspeed * cos(player.dir);
		player.y -= movementspeed * sin(player.dir);
	}
	if (keypresses & KEY_a) {
		player.x += movementspeed * cos(player.dir + PI / 2);
		player.y -= movementspeed * sin(player.dir + PI / 2);
	}
	if (keypresses & KEY_s | keypresses & KEY_o | keypresses & KEY_DOWN) {
		player.x -= movementspeed * cos(player.dir);
		player.y += movementspeed * sin(player.dir);
	}
	if (keypresses & KEY_d | keypresses & KEY_e) {
		player.x += movementspeed * cos(player.dir - PI / 2);
		player.y -= movementspeed * sin(player.dir - PI / 2);
	}
	if (keypresses & KEY_LEFT) {
		player.dir += turnspeed;
	}
	if (keypresses & KEY_RIGHT) {
		player.dir -= turnspeed;
	}
}

void render (SDL_Renderer *renderer)
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_RenderClear(renderer);

	static const int mapscale = WINDOW_HEIGHT / MAP_HEIGHT;
	SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0);
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			if (!map[i][j]) continue;
			SDL_RenderFillRect(renderer, &(SDL_Rect){
				.x = j * mapscale,
				.y = i * mapscale,
				.w = mapscale,
				.h = mapscale,
			});
		}
	}

	SDL_SetRenderDrawColor(renderer, 0x88, 0x88, 0x88, 0);
	SDL_RenderFillRectF(renderer, &(SDL_FRect){
		.x = player.x * mapscale - mapscale * 0.05,
		.y = player.y * mapscale - mapscale * 0.05,
		.w = mapscale * 0.1,
		.h = mapscale * 0.1,
	});
	SDL_RenderDrawLineF(renderer,
		player.x * mapscale, player.y * mapscale,
		(player.x + cos(player.dir) * 0.1) * mapscale, (player.y - sin(player.dir) * 0.1) * mapscale
	);

	SDL_RenderPresent(renderer);
}
