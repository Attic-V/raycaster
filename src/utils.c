#include "utils.h"

int min (int a, int b)
{
	return a * (a <= b) + b * (b < a);
}

int max (int a, int b)
{
	return a * (a >= b) + b * (b > a);
}
