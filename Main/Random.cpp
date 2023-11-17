#include <ctime>
#include <cstdlib>
#include "Random.h"

void Random::Initialize()
{
	srand(static_cast<unsigned int>(time(nullptr)));
}

int Random::Int(int start, int end)
{
	int value = (rand() + start) % (end + 1);

	return value < start ? start : value;
}