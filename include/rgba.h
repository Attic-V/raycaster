#ifndef rgba_h
#define rgba_h

#include <stdint.h>

uint16_t rgba_8888to4444 (uint32_t rgba8888);
uint32_t rgba_4444to8888 (uint16_t rgba4444);

#endif
