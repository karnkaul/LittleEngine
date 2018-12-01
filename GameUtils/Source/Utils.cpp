#include <ctime>
#include <algorithm>
#include <stack>
#include "Utils.h"

namespace Maths {
	bool _bInit = false;
	static void InitRand(bool useTime = true) {
		if (useTime) {
			srand(static_cast<unsigned int>(time(nullptr)));
		}
		else {
			srand(0);
		}
		_bInit = true;
	}

	Fixed Random::Range(const Fixed& min, const Fixed& max) {
		if (!_bInit) {
			InitRand();
		}
		int random = rand();
		Fixed normalised(random, RAND_MAX);
		return (normalised * (max - min)) + min;
	}

	int Random::Range(int min, int max) {
		if (!_bInit) {
			InitRand();
		}
		return ((rand() * (max - min)) / RAND_MAX) + min;
	}

	size_t Random::Range(size_t min, size_t max) {
		if (!_bInit) {
			InitRand();
		}
		return ((rand() * (max - min)) / RAND_MAX) + min;
	}

	Fixed Lerp(const Fixed& a, const Fixed& b, Fixed t) {
		t = Clamp01(t);
		return ((Fixed::One - t) * a) + (t * b);
	}
}

namespace Strings {
	void ToLower(std::string& lhs) {
		std::transform(lhs.begin(), lhs.end(), lhs.begin(), ::tolower);
	}

	bool ToBool(const std::string& input, bool bDefaultValue) {
		std::string inputLower(input);
		ToLower(inputLower);
		if (inputLower == "true" || inputLower == "1") {
			return true;
		}
		return bDefaultValue;
	}

	int ToInt(const std::string& input, int defaultValue) {
		int ret;
		try {
			ret = std::stoi(input);
		}
		catch (std::exception&) {
			ret = defaultValue;
		}
		return ret;
	}

	double ToDouble(const std::string & input, double defaultValue) {
		double ret;
		try {
			ret = std::stod(input);
		}
		catch (std::exception&) {
			ret = defaultValue;
		}
		return ret;
	}

	std::string ToString(bool bInput) {
		return bInput ? "true" : "false";
	}

	Pair<std::string> Bisect(const std::string & input, char delimiter) {
		size_t idx = input.find(delimiter);
		std::string rhs = idx < input.size() ? input.substr(idx + 1) : "";
		return Pair(input.substr(0, idx), std::move(rhs));
	}

	void RemoveChars(std::string & outInput, std::initializer_list<char> toRemove) {
		std::string::iterator iter = std::remove_if(outInput.begin(), outInput.end(), 
			[&toRemove](char c) -> bool {
				for (const auto& x : toRemove) {
					if (c == x) return true;
				}
				return false;
			}
		);
		outInput.erase(iter, outInput.end());
	}

	void RemoveWhitespace(std::string& outInput) {
		SubstituteChars(outInput, { Pair('\t', ' '), Pair('\n', ' ') });
		RemoveChars(outInput, { ' ' });
	}

	std::vector<std::string> Tokenise(const std::string& s, const char delimiter, std::initializer_list<Pair<char>> escape) {
		auto end = s.cend();
		auto start = end;

		std::stack<Pair<char>> escapeStack;
		std::vector<std::string> v;
		bool escaping = false;
		for (auto it = s.cbegin(); it != end; ++it) {
			if (*it != delimiter || escaping) {
				if (start == end)
					start = it;
				for (auto e : escape) {
					if (*it == e.first) {
						escaping = true;
						escapeStack.push(e);
						break;
					}
					else if (*it == e.second) {
						if (e.first == escapeStack.top().first) {
							escapeStack.pop();
							escaping = !escapeStack.empty();
							break;
						}
					}
				}
				continue;
			}
			if (start != end) {
				v.emplace_back(start, it);
				start = end;
			}
		}
		if (start != end) {
			v.emplace_back(start, end);
		}
		return v;
	}

	void SubstituteChars(std::string & outInput, std::initializer_list<Pair<char>> replacements) {
		std::string::iterator iter = outInput.begin();
		while (iter != outInput.end()) {
			for (const auto replacement : replacements) {
				if (*iter == replacement.first) {
					*iter = replacement.second;
					break;
				}
			}
			++iter;
		}
	}
}
