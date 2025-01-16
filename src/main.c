#include <stdbool.h>

#include <SDL2/SDL.h>

#include "map.h"
#include "player.h"
#include "window.h"

#define PI 3.1415926535897932384626433

void move (double frameTime);
void render (SDL_Window *window, SDL_Renderer *renderer);

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

	double time = 0;
	double oldTime = 0;

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

		oldTime = time;
		time = SDL_GetTicks64();
		double frameTime = (time - oldTime) / 1000.0;

		move(frameTime);

		render(window, renderer);
	}

	SDL_DestroyWindow(window);

	SDL_Quit();
}

void move (double frameTime)
{
	double movementspeed = 1.5 * frameTime;
	double turnspeed = 2 * PI * 0.25 * frameTime;

	bool move_forwards = (keypresses & KEY_w) | (keypresses & KEY_COMMA) | (keypresses & KEY_UP);
	bool move_left = (keypresses & KEY_a);
	bool move_backwards = (keypresses & KEY_s) | (keypresses & KEY_o) | (keypresses & KEY_DOWN);
	bool move_right = (keypresses & KEY_d) | (keypresses & KEY_e);
	bool turn_left = (keypresses & KEY_LEFT);
	bool turn_right = (keypresses & KEY_RIGHT);

	double move_x = movementspeed * (
		move_forwards * cos(player.dir) +
		move_left * cos(player.dir + PI / 2) +
		move_right * cos(player.dir - PI / 2) +
		move_backwards * -cos(player.dir)
	);
	double move_y = movementspeed * (
		move_forwards * sin(player.dir) +
		move_left * sin(player.dir + PI / 2) +
		move_right * sin(player.dir - PI / 2) +
		move_backwards * -sin(player.dir)
	);

	player.x += move_x * !map[(int)player.y][(int)(player.x + move_x)];
	player.y -= move_y * !map[(int)(player.y - move_y)][(int)player.x];
	player.dir += turnspeed * turn_left;
	player.dir -= turnspeed * turn_right;
}

void render (SDL_Window *window, SDL_Renderer *renderer)
{
	int w, h;
	SDL_GetWindowSize(window, &w, &h);

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_RenderClear(renderer);

	static const double HFOV = PI / 2; // cannot be a multiple of π
	double VFOV = 2 * atan(tan(HFOV / 2) * ((double)h / w));
	for (int i = 0; i < w; i++) {
		double cameraWidth = 2 * tan(HFOV / 2);
		double cameraX = cameraWidth * i / (double)w - cameraWidth / 2;

		double dir = player.dir + PI / 2 - atan2(1, cameraX * (HFOV > PI ? -1 : 1));

		int mapX = player.x;
		int mapY = player.y;

		double diffX = player.x - mapX;
		double diffY = player.y - mapY;

		double currentY, currentX;
		double distX, distY;

		currentY = player.y;
		currentX = player.x;
		if (cos(dir) < 0) {
			currentX -= diffX;
			currentY += diffX * tan(dir);
			while (0 < currentY && currentY < MAP_HEIGHT && !map[(int)currentY][(int)currentX - 1]) {
				currentX--;
				currentY += tan(dir);
			}
		} else if (cos(dir) > 0) {
			currentX += 1 - diffX;
			currentY -= (1 - diffX) * tan(dir);
			while (0 < currentY && currentY < MAP_HEIGHT && !map[(int)currentY][(int)currentX]) {
				currentX++;
				currentY -= tan(dir);
			}
		} else {
			currentX = 1e30;
		}
		distX = sqrt((player.x - currentX) * (player.x - currentX) + (player.y - currentY) * (player.y - currentY));

		currentY = player.y;
		currentX = player.x;
		if (sin(dir) > 0) {
			currentY -= diffY;
			currentX += diffY / tan(dir);
			while (0 < currentX && currentX < MAP_WIDTH && !map[(int)currentY - 1][(int)currentX]) {
				currentY--;
				currentX += 1 / tan(dir);
			}
		} else if (sin(dir) < 0) {
			currentY += 1 - diffY;
			currentX -= (1 - diffY) / tan(dir);
			while (0 < currentX && currentX < MAP_WIDTH && !map[(int)currentY][(int)currentX]) {
				currentY++;
				currentX -= 1 / tan(dir);
			}
		} else {
			currentY = 1e30;
		}
		distY = sqrt((player.x - currentX) * (player.x - currentX) + (player.y - currentY) * (player.y - currentY));

		currentY = player.y;
		currentX = player.x;
		double trueDist = distX < distY ? distX : distY;
		double correctedDist = trueDist * cos(player.dir - dir);
		int xSide = distX < distY;
		currentX += trueDist * cos(dir);
		currentY -= trueDist * sin(dir);

		double maxDist = sqrt(MAP_WIDTH * MAP_WIDTH + MAP_HEIGHT * MAP_HEIGHT);
		double shade = (maxDist - trueDist) / maxDist;

		static const double wallHeight = 1;
		double cameraHeight = 2 * tan(VFOV / 2);
		double windowY = (wallHeight / 2) / correctedDist;
		double screenWallHeight = h * (windowY * 2) / cameraHeight;

		SDL_SetRenderDrawColor(renderer, (xSide ? 0xff : 0xdd) * shade, 0, 0, 0);
		SDL_RenderFillRectF(renderer, &(SDL_FRect){
			.x = w - i - 1,
			.y = h / 2 - screenWallHeight / 2,
			.w = 1,
			.h = screenWallHeight,
		});
	}

	SDL_RenderPresent(renderer);
}
