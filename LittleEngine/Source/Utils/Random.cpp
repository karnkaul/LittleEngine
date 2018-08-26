#include "stdafx.h"
#include <ctime>
#include "Random.h"

bool _init = false;
void InitRand(bool useTime = true) {
	if (useTime) {
		srand(static_cast<unsigned int>(time(nullptr)));
	}
	else {
		srand(0);
	}
	_init = true;
}

Fixed Random::Range(Fixed min, Fixed max) {
	if (!_init) {
		InitRand();
	}
	int random = rand();
	Fixed normalised(random, RAND_MAX);
	return normalised * (max - min);
}

int Random::Range(int min, int max) {
	if (!_init) {
		InitRand();
	}
	return (rand() * (max - min)) / RAND_MAX;
}
