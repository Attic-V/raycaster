#ifndef texture_h
#define texture_h

#include <stdint.h>

#define TEXTURE_SIZE 256
#define TEXTURE_COUNT 8

extern uint32_t textures[TEXTURE_COUNT][TEXTURE_SIZE][TEXTURE_SIZE];
extern uint32_t texcolor[TEXTURE_COUNT];

void initTextures (void);

#endif
