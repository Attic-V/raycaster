#include <stdlib.h>

#include "rgba.h"
#include "texture.h"
#include "utils.h"

void initTexColors (void);

RGBA8888 textures[TEXTURE_COUNT][TEXTURE_SIZE][TEXTURE_SIZE];

void initTextures (void)
{
	for (int y = 0; y < TEXTURE_SIZE; y++) {
		for (int x = 0; x < TEXTURE_SIZE; x++) {
			{
				const int lineWidth = TEXTURE_SIZE / 64;
				if (abs(y - x) > lineWidth && abs(TEXTURE_SIZE - x - y) > lineWidth) {
					textures[0][x][y] = 0xff0044ff;
				} else {
					textures[0][x][y] = 0xffffffff;
				}
			}
			{
				int c = ((y + x) % TEXTURE_SIZE) / ((double)TEXTURE_SIZE / 256);
				textures[1][x][y] = ((0x44 - c) << 24) | ((0x88 - c) << 16) | ((0xff - c) << 8) | 0xff;
			}
			{
				int c = y / ((double)TEXTURE_SIZE / 256);
				textures[2][x][y] = ((0x44 - c) << 24) | ((0x88 - c) << 16) | ((0xff - c) << 8) | 0xff;
			}
			{
				const int mortar = max(TEXTURE_SIZE / 128, TEXTURE_SIZE >= 32 ? 1 : 0);
				const int brickH = TEXTURE_SIZE / 16;
				const int brickW = brickH * 2;
				const double offset = 0.8;
				RGBA8888 white = (rand() % (0xdd - 0xbb + 1) + 0xbb) * 0x01010100 | 0xff;
				RGBA8888 red = (rand() % (0xbb - 0x55 + 1) + 0x55) << 24 | 0x003020ff;
				if ((y + mortar / 2) % brickH < mortar) {
					textures[3][x][y] = white;
				} else {
					if ((x + mortar / 2 + (int)((y) / brickH * brickH * offset)) % brickW < mortar) {
						textures[3][x][y] = white;
					} else {
						textures[3][x][y] = red;
					}
				}

				if (((x + (int)(y / brickH * brickH * offset) % brickW) / brickW * brickW) < TEXTURE_SIZE * 0.2) {
					textures[4][x][y] = textures[3][x][y];
					textures[5][x][y] = textures[0][x][y];
				} else {
					textures[4][x][y] = textures[0][x][y];
					textures[5][x][y] = textures[3][x][y];
				}
			}
			{
				if ((x > y && x > TEXTURE_SIZE - y) || (TEXTURE_SIZE - x > y && TEXTURE_SIZE - x > TEXTURE_SIZE - y)) {
					textures[6][x][y] = 0xffffffff;
					textures[8][x][y] = 0x008888ff;
				} else {
					textures[6][x][y] = 0x008888ff;
					textures[8][x][y] = 0xffffffff;
				}
			}
			{
				if (x + TEXTURE_SIZE / 2 > y && x < y + TEXTURE_SIZE / 2 && TEXTURE_SIZE - x + TEXTURE_SIZE / 2 > y && TEXTURE_SIZE - x < y + TEXTURE_SIZE / 2) {
					textures[7][x][y] = 0xffffffff;
					textures[9][x][y] = 0x008888ff;
				} else {
					textures[7][x][y] = 0x008888ff;
					textures[9][x][y] = 0xffffffff;
				}
			}
		}
	}
}

