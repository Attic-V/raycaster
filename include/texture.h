#ifndef texture_h
#define texture_h

#include <stdint.h>

#include "rgba.h"

#define TEXTURE_SIZE 256
#define TEXTURE_COUNT 10

extern RGBA8888 textures[TEXTURE_COUNT][TEXTURE_SIZE][TEXTURE_SIZE];

void initTextures (void);

#endif
