#include "rgba.h"

RGBA4444 rgba_8888to4444 (RGBA8888 rgba)
{
	return
		((rgba & 0xf0000000) >> 16) |
		((rgba & 0x00f00000) >> 12) |
		((rgba & 0x0000f000) >>  8) |
		((rgba & 0x000000f0) >>  4)
	;
}

RGBA8888 rgba_4444to8888 (RGBA4444 rgba)
{
	return
		((rgba & 0xf000) * 0x00011000) |
		((rgba & 0x0f00) * 0x00001100) |
		((rgba & 0x00f0) * 0x00000110) |
		((rgba & 0x000f) * 0x00000011)
	;
}

RGBA8888 rgba_channelReduce (RGBA8888 rgba, RgbaChannel channel, double percent)
{
	RGBA8888 mask = channel;

	uint8_t shift;
	switch (channel) {
		case RgbaChannel_R: shift = 24; break;
		case RgbaChannel_G: shift = 16; break;
		case RgbaChannel_B: shift =  8; break;
		case RgbaChannel_A: shift =  0; break;
		default:
			return rgba;
	}

	return (rgba & ~mask) | ((uint8_t)(percent * ((rgba & mask) >> shift)) << shift);
}
