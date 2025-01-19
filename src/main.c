#include <stdbool.h>

#include <SDL2/SDL.h>

#include "map.h"
#include "player.h"
#include "utils.h"
#include "window.h"

#define PI 3.1415926535897932384626433

#define TEXTURE_SIZE 256
#define TEXTURE_COUNT 4

void initTextures (void);
void move (double frameTime);
void render (SDL_Window *window, SDL_Renderer *renderer);
void setRenderDrawColor (SDL_Renderer *renderer, uint32_t color);

bool texturesOn = true;
uint32_t textures[TEXTURE_COUNT][TEXTURE_SIZE][TEXTURE_SIZE];

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
	initTextures();

	SDL_Init(SDL_INIT_VIDEO);

	SDL_Window *window = SDL_CreateWindow(
		"raycaster",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		WINDOW_WIDTH, WINDOW_HEIGHT,
		SDL_WINDOW_BORDERLESS
	);

	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

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
				case SDLK_SEMICOLON: texturesOn = !texturesOn; break;
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

void initTextures (void)
{
	for (int y = 0; y < TEXTURE_SIZE; y++) {
		for (int x = 0; x < TEXTURE_SIZE; x++) {
			{
				int c = 0xff * (abs(y - x) > 4);
				textures[0][x][y] = c << 24 | 0xff;
			}
			{
				int c = (y + x) % TEXTURE_SIZE;
				textures[1][x][y] = ((0x44 - c) << 24) | ((0x88 - c) << 16) | ((0xff - c) << 8) | 0xff;
			}
			{
				int c = y;
				textures[2][x][y] = ((0x44 - c) << 24) | ((0x88 - c) << 16) | ((0xff - c) << 8) | 0xff;
			}
			{
				const int mortar = TEXTURE_SIZE / 128;
				const int brickH = TEXTURE_SIZE / 16;
				const int brickW = brickH * 2;
				uint32_t white = (rand() % (0xdd - 0xbb + 1) + 0xbb) * 0x01010100 | 0xff;
				uint32_t red = (rand() % (0xbb - 0x55 + 1) + 0x55) << 24 | 0x003020ff;
				if ((y + mortar / 2) % brickH < mortar) {
					textures[3][x][y] = white;
				} else {
					if ((x + mortar / 2 + (int)((y) / brickH * brickH * 0.8)) % brickW < mortar) {
						textures[3][x][y] = white;
					} else {
						textures[3][x][y] = red;
					}
				}
			}
		}
	}
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

	SDL_PixelFormatEnum pixelformat = SDL_PIXELFORMAT_RGBA8888;

	SDL_Texture *texture = SDL_CreateTexture(renderer, pixelformat, texturesOn ? SDL_TEXTUREACCESS_STREAMING : SDL_TEXTUREACCESS_TARGET, w, h);

	void *pixels;
	int pitch;
	if (texturesOn) {
		SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
		SDL_LockTexture(texture, NULL, &pixels, &pitch);
	}

	static const double HFOV = PI / 2; // cannot be a multiple of π
	double VFOV = 2 * atan(tan(HFOV / 2) * ((double)h / w));
	for (int i = 0; i < w; i++) {
		double cameraWidth = 2 * tan(HFOV / 2);
		double cameraX = cameraWidth * i / (double)w - cameraWidth / 2;

		double dir = player.dir + PI / 2 - atan2(1, cameraX * (HFOV > PI ? -1 : 1));

		int mapX = player.x;
		int mapY = player.y;

		double dirX = cos(dir);
		double dirY = sin(dir);

		double deltaX = dirX == 0 ? 1e30 : fabs(1 / dirX);
		double deltaY = dirY == 0 ? 1e30 : fabs(1 / dirY);

		double distX = deltaX * (dirX < 0 ? player.x - mapX : 1 - player.x + mapX);
		double distY = deltaY * (dirY > 0 ? player.y - mapY : 1 - player.y + mapY);

		int sX = dirX < 0 ? -1 : 1;
		int sY = dirY > 0 ? -1 : 1;

		int side = 0;
		while (!map[mapY][mapX]) {
			if (distX < distY) {
				mapX += sX;
				distX += deltaX;
				side = 0;
			} else {
				mapY += sY;
				distY += deltaY;
				side = 1;
			}
		}

		double trueDist = side == 0 ? distX - deltaX : distY - deltaY;
		double dist = trueDist * cos(player.dir - dir);
		double maxDist = sqrt((MAP_WIDTH - 2) * (MAP_WIDTH - 2) + (MAP_HEIGHT - 2) * (MAP_HEIGHT - 2));
		double percentMaxDist = (maxDist - trueDist) / maxDist;

		static const double wallHeight = 1;
		double cameraHeight = 2 * tan(VFOV / 2);
		double windowY = (wallHeight / 2) / dist;
		double screenWallHeight = h * (windowY * 2) / cameraHeight;

		int type = map[mapY][mapX];

		int lineStart = h / 2 - screenWallHeight / 2;
		int lineEnd = h / 2 + screenWallHeight / 2;
		if (texturesOn) {
			double collideX = player.x + cos(dir) * trueDist;
			double collideY = player.y - sin(dir) * trueDist;

			double wallX =
				side == 0
					? (sX < 0 ? 1 - collideY + (int)collideY : collideY - (int)collideY)
					: (sY < 0 ? collideX - (int)collideX : 1 - collideX + (int)collideX)
				;
			wallX = fabs(wallX);

			int x = wallX * TEXTURE_SIZE;
			uint32_t *column = textures[type - 1][x];

			uint32_t *upixels = (uint32_t *)pixels;
			for (int j = max(lineStart, 0); j < min(lineEnd, h); j++) {
				int y = (double)(j - lineStart) / (lineEnd - lineStart) * TEXTURE_SIZE;
				int c = column[y];
				if (side) {
					uint8_t a = c & 0xff;
					a *= 0.9;
					c &= 0xffffff00;
					c |= a;
				};
				uint8_t a = c & 0xff;
				a *= percentMaxDist;
				c &= 0xffffff00;
				c |= a;

				SDL_PixelFormat *format = SDL_AllocFormat(pixelformat);
				upixels[w - i - 1 + j * w] = SDL_MapRGBA(format,
					(c & 0xff000000) >> 24,
					(c & 0x00ff0000) >> 16,
					(c & 0x0000ff00) >> 8,
					c & 0xff
				);
			}
		} else {
			uint32_t color;
			switch (type) {
				case 1: color = 0xff0000ff; break;
				case 2: color = 0x00ff00ff; break;
				case 3: color = 0x0000ffff; break;
				default: color = 0xffffffff;
			}
			if (side) {
				uint8_t a = color & 0xff;
				a *= 0.9;
				color &= 0xffffff00;
				color |= a;
			};
			uint8_t a = color & 0xff;
			a *= percentMaxDist;
			color &= 0xffffff00;
			color |= a;
			SDL_SetRenderTarget(renderer, texture);
			setRenderDrawColor(renderer, color);
			SDL_RenderDrawLineF(renderer,
				w - i - 1, max(lineStart, 0),
				w - i - 1, min(lineEnd, h)
			);
			SDL_SetRenderTarget(renderer, NULL);
		}
	}

	if (texturesOn) {
		SDL_UnlockTexture(texture);
	}
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_DestroyTexture(texture);
	texture = NULL;

	SDL_RenderPresent(renderer);
}

void setRenderDrawColor (SDL_Renderer *renderer, uint32_t color)
{
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer,
		(color & 0xff000000) >> 8 * 3,
		(color & 0x00ff0000) >> 8 * 2,
		(color & 0x0000ff00) >> 8 * 1,
		(color & 0x000000ff) >> 8 * 0
	);
}
