#include <ctime>
#include "Utils.h"

namespace Maths {
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
		return (normalised * (max - min)) + min;
	}

	int Random::Range(int min, int max) {
		if (!_init) {
			InitRand();
		}
		return ((rand() * (max - min)) / RAND_MAX) + min;
	}
}

namespace Strings {
	int ToInt(const std::string& input, int defaultValue) {
		int ret;
		try {
			ret = std::stoi(input);
		}
		catch (std::exception e) {
			ret = defaultValue;
		}
		return ret;
	}
}
