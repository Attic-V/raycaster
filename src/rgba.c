#include "rgba.h"

uint16_t rgba_8888to4444 (uint32_t rgba8888)
{
	return
		((rgba8888 & 0xf0000000) >> 16) |
		((rgba8888 & 0x00f00000) >> 12) |
		((rgba8888 & 0x0000f000) >>  8) |
		((rgba8888 & 0x000000f0) >>  4)
	;
}

uint32_t rgba_4444to8888 (uint16_t rgba4444)
{
	return
		((rgba4444 & 0xf000) * 0x00011000) |
		((rgba4444 & 0x0f00) * 0x00001100) |
		((rgba4444 & 0x00f0) * 0x00000110) |
		((rgba4444 & 0x000f) * 0x00000011)
	;
}

uint32_t rgba_channelReduce (uint32_t rgba8888, RgbaChannel channel, double percent)
{
	uint32_t mask = channel;

	uint8_t shift;
	switch (channel) {
		case RgbaChannel_R: shift = 24; break;
		case RgbaChannel_G: shift = 16; break;
		case RgbaChannel_B: shift =  8; break;
		case RgbaChannel_A: shift =  0; break;
		default:
			return rgba8888;
	}

	return (rgba8888 & ~mask) | ((uint8_t)(percent * ((rgba8888 & mask) >> shift)) << shift);
}
