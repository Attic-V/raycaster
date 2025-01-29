#ifndef rgba_h
#define rgba_h

#include <stdint.h>

#define rgba_alphaReduce(rgba8888, percent) \
	rgba_channelReduce(rgba8888, RgbaChannel_A, percent)

typedef enum {
	RgbaChannel_R = 0xff000000,
	RgbaChannel_G = 0x00ff0000,
	RgbaChannel_B = 0x0000ff00,
	RgbaChannel_A = 0x000000ff,
} RgbaChannel;

uint16_t rgba_8888to4444 (uint32_t rgba8888);
uint32_t rgba_4444to8888 (uint16_t rgba4444);
uint32_t rgba_channelReduce (uint32_t rgba8888, RgbaChannel channel, double percent);

#endif
