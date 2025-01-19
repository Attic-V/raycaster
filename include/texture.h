#ifndef texture_h
#define texture_h

#include <stdint.h>

#define TEXTURE_SIZE 256
#define TEXTURE_COUNT 6

extern uint32_t textures[TEXTURE_COUNT][TEXTURE_SIZE][TEXTURE_SIZE];

void initTextures (void);

#endif
