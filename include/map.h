#ifndef map_h
#define map_h

#define MAP_WIDTH 8
#define MAP_HEIGHT 8

uint8_t map[MAP_WIDTH][MAP_HEIGHT] = {
	{1,1,1,1,1,1,1,1},
	{1,0,0,0,0,0,0,1},
	{1,0,1,0,0,2,0,1},
	{1,0,4,0,0,2,0,1},
	{1,0,0,0,3,3,0,1},
	{1,0,0,0,3,0,0,1},
	{1,0,0,0,0,0,0,1},
	{1,1,1,1,1,1,1,1},
};

#endif
