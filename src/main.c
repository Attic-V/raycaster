#include <stdbool.h>

#include <SDL2/SDL.h>

#include "map.h"
#include "window.h"

void render (SDL_Renderer *renderer);

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

	bool quit = false;
	while (!quit) {
		SDL_Event event;
		SDL_PollEvent(&event);
		quit =
			(event.type == SDL_QUIT) ||
			(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
		;

		render(renderer);
	}

	SDL_DestroyWindow(window);

	SDL_Quit();
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

	SDL_RenderPresent(renderer);
}
