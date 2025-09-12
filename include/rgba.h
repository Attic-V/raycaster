#ifndef rgba_h
#define rgba_h

#include <stdint.h>

#define rgba_alphaReduce(rgba8888, percent) \
	rgba_channelReduce((rgba8888), RgbaChannel_A, (percent))

typedef uint32_t RGBA8888;
typedef uint16_t RGBA4444;

typedef enum : uint32_t {
	RgbaChannel_R = 0xff000000,
	RgbaChannel_G = 0x00ff0000,
	RgbaChannel_B = 0x0000ff00,
	RgbaChannel_A = 0x000000ff,
} RgbaChannel;

RGBA4444 rgba_8888to4444 (RGBA8888 rgba);
RGBA8888 rgba_4444to8888 (RGBA4444 rgba);
RGBA8888 rgba_channelReduce (RGBA8888 rgba, RgbaChannel channel, double percent);

#endif
