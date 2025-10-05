#include <stdbool.h>

#include <SDL2/SDL.h>

#include "map.h"
#include "player.h"
#include "rgba.h"
#include "texture.h"
#include "utils.h"
#include "window.h"

#define PI 3.1415926535897932384626433

void move (double frameTime);
void render (SDL_Window *window, SDL_Renderer *renderer);
void setRenderDrawColor (SDL_Renderer *renderer, uint32_t color);

Player player;

typedef enum {
	#define KEY(key, idx) \
		KEY_##key = (1 << idx)
	KEY(w, 0),
	KEY(a, 1),
	KEY(s, 2),
	KEY(d, 3),
	KEY(COMMA, 4),
	KEY(o, 5),
	KEY(e, 6),
	KEY(UP, 7),
	KEY(LEFT, 8),
	KEY(DOWN, 9),
	KEY(RIGHT, 10),
	#undef KEY
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
		.dir = -PI / 9,
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
				#define SET(key) \
					case SDLK_##key: keypresses |= KEY_##key; break;
				SET(w)
				SET(a)
				SET(s)
				SET(d)
				SET(COMMA)
				SET(o)
				SET(e)
				SET(UP)
				SET(LEFT)
				SET(DOWN)
				SET(RIGHT)
				#undef SET
			}
		}
		if (event.type == SDL_KEYUP) {
			switch (event.key.keysym.sym) {
				#define UNSET(key) \
					case SDLK_##key: keypresses &= ~0 ^ KEY_##key; break;
				UNSET(w)
				UNSET(a)
				UNSET(s)
				UNSET(d)
				UNSET(COMMA)
				UNSET(o)
				UNSET(e)
				UNSET(UP)
				UNSET(LEFT)
				UNSET(DOWN)
				UNSET(RIGHT)
				#undef UNSET
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

	#define KEYSTATE(key) (keypresses & KEY_##key)

	bool move_forwards = KEYSTATE(w) | KEYSTATE(COMMA) | KEYSTATE(UP);
	bool move_left = KEYSTATE(a);
	bool move_backwards = KEYSTATE(s) | KEYSTATE(o) | KEYSTATE(DOWN);
	bool move_right = KEYSTATE(d) | KEYSTATE(e);
	bool turn_left = KEYSTATE(LEFT);
	bool turn_right = KEYSTATE(RIGHT);

	#undef KEYSTATE

	double move_x = movementspeed * (
		move_forwards  *  cos(player.dir) +
		move_left      *  cos(player.dir + PI / 2) +
		move_right     *  cos(player.dir - PI / 2) +
		move_backwards * -cos(player.dir)
	);
	double move_y = movementspeed * (
		move_forwards  *  sin(player.dir) +
		move_left      *  sin(player.dir + PI / 2) +
		move_right     *  sin(player.dir - PI / 2) +
		move_backwards * -sin(player.dir)
	);

	player.x += move_x * !map[(int)player.y][(int)(player.x + move_x)];
	player.y -= move_y * !map[(int)(player.y - move_y)][(int)player.x];
	player.dir += turnspeed * turn_left;
	player.dir -= turnspeed * turn_right;
}

void render (SDL_Window *window, SDL_Renderer *renderer)
{
	#define RENDER_PIXEL(x, y, rgba8888) \
		do { \
			upixels[(x) + (y) * w] = SDL_MapRGBA(format, \
				((rgba8888) & 0xff000000) >> 24, \
				((rgba8888) & 0x00ff0000) >> 16, \
				((rgba8888) & 0x0000ff00) >>  8, \
				((rgba8888) & 0x000000ff) >>  0  \
			); \
		} while (0)

	int w, h;
	SDL_GetWindowSize(window, &w, &h);

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_RenderClear(renderer);

	SDL_PixelFormatEnum pixelformat = SDL_PIXELFORMAT_RGBA8888;
	SDL_PixelFormat *format = SDL_AllocFormat(pixelformat);

	SDL_Texture *texture = SDL_CreateTexture(renderer, pixelformat, SDL_TEXTUREACCESS_STREAMING, w, h);

	void *pixels;
	int pitch;
	SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
	SDL_LockTexture(texture, NULL, &pixels, &pitch);
	uint32_t *upixels = (uint32_t *)pixels;

	double vanishDist = 5;
	double wallBrightnessDiff = 0.9;

	static const double HFOV = PI / 2;
	double VFOV = fabs(2 * atan(tan(HFOV / 2) * ((double)h / w)));

	double cameraWidth = 2 * tan(HFOV / 2);
	double cameraHeight = 2 * tan(VFOV / 2);

	static const double wallHeight = 1;

	for (int i = 0; i < w; i++) {
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
			if (fmax(distX - deltaX, distY - deltaY) > vanishDist) break;
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
		double percentVanishDist = (vanishDist - trueDist) / vanishDist;
		if (trueDist > vanishDist) percentVanishDist = 0;

		double windowY = (wallHeight / 2) / dist;
		double screenWallHeight = h * (windowY * 2) / cameraHeight;

		int type = map[mapY][mapX];

		int lineStart = h / 2 - screenWallHeight / 2;
		int lineEnd = h / 2 + screenWallHeight / 2 + 1;

		if (percentVanishDist > 0) {
			double collideX = player.x + dirX * trueDist;
			double collideY = player.y - dirY * trueDist;

			double wallX =
				side == 0
					? (sX < 0 ? 1 - collideY + (int)collideY : collideY - (int)collideY)
					: (sY < 0 ? collideX - (int)collideX : 1 - collideX + (int)collideX)
				;
			wallX = fabs(wallX);

			int x = wallX * TEXTURE_SIZE;
			RGBA8888 *column = textures[type - 1][x];

			RGBA8888 color;

			for (int j = max(lineStart, 0); j < min(lineEnd, h); j++) {
				int y = (double)(j - lineStart) / (lineEnd - lineStart) * TEXTURE_SIZE;

				color = column[y];
				color = rgba_alphaReduce(color, percentVanishDist * (side ? wallBrightnessDiff : 1));

				RENDER_PIXEL(w - i - 1, j, color);
			}
		}

		for (int y = lineEnd; y < h; y++) {
			double screenPos = 2 * y - h;
			double windowY = (screenPos * cameraHeight) / (h * 2);
			double dist = wallHeight / (2 * windowY);
			double trueDist = dist / cos(player.dir - dir);

			double percentVanishDist = (vanishDist - trueDist) / vanishDist;
			if (trueDist > vanishDist) continue;

			double collideX = player.x + trueDist * dirX;
			double collideY = player.y - trueDist * dirY;

			int mapX = collideX;
			int mapY = collideY;

			double tileX = collideX - mapX;
			double tileY = collideY - mapY;

			int texX = tileX * TEXTURE_SIZE;
			int texY = tileY * TEXTURE_SIZE;

			int floorTex = 6;
			int ceilingTex =
				mapX % 2 == 0
				? (mapX + mapY) % 2 == 0 ? 6 : 7
				: (mapX + mapY) % 2 == 0 ? 8 : 9
			;

			RGBA8888 floorColor = textures[floorTex][texX][texY];
			RGBA8888 ceilingColor = textures[ceilingTex][texX][texY];

			floorColor = rgba_alphaReduce(floorColor, percentVanishDist);
			ceilingColor = rgba_alphaReduce(ceilingColor, percentVanishDist);

			RENDER_PIXEL(w - i - 1, y, floorColor);
			RENDER_PIXEL(w - i - 1, h - y - 1, ceilingColor);
		}
	}

	SDL_UnlockTexture(texture);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_DestroyTexture(texture);
	texture = NULL;

	SDL_RenderPresent(renderer);

	#undef RENDER_PIXEL
}

void setRenderDrawColor (SDL_Renderer *renderer, RGBA8888 color)
{
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer,
		(color & 0xff000000) >> 8 * 3,
		(color & 0x00ff0000) >> 8 * 2,
		(color & 0x0000ff00) >> 8 * 1,
		(color & 0x000000ff) >> 8 * 0
	);
}
