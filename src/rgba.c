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
