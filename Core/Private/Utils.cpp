#include "stdafx.h"
#include "Utils.h"
#include <algorithm>
#include <ctime>
#include <stack>

namespace Strings
{
void ToLower(String& lhs)
{
	std::transform(lhs.begin(), lhs.end(), lhs.begin(), ::tolower);
}

bool ToBool(const String& input, bool bDefaultValue)
{
	String inputLower(input);
	ToLower(inputLower);
	if (inputLower == "true" || inputLower == "1")
	{
		return true;
	}
	return bDefaultValue;
}

s32 ToS32(const String& input, s32 defaultValue)
{
	s32 ret;
	try
	{
		ret = std::stoi(input);
	}
	catch (const std::exception&)
	{
		ret = defaultValue;
	}
	return ret;
}

f32 ToF32(const String& input, f32 defaultValue)
{
	f32 ret;
	try
	{
		ret = std::stof(input);
	}
	catch (const std::exception&)
	{
		ret = defaultValue;
	}
	return ret;
}

f64 ToF64(const String& input, f64 defaultValue)
{
	f64 ret;
	try
	{
		ret = std::stod(input);
	}
	catch (const std::exception&)
	{
		ret = defaultValue;
	}
	return ret;
}

String ToString(bool bInput)
{
	return bInput ? "true" : "false";
}

Pair<String> Bisect(const String& input, char delimiter)
{
	size_t idx = input.find(delimiter);
	String rhs = idx < input.size() ? input.substr(idx + 1) : "";
	return Pair<String>(input.substr(0, idx), std::move(rhs));
}

void RemoveChars(String& outInput, InitList<char> toRemove)
{
	String::iterator iter = std::remove_if(outInput.begin(), outInput.end(), [&toRemove](char c) -> bool {
		for (const auto& x : toRemove)
		{
			if (c == x)
				return true;
		}
		return false;
	});
	outInput.erase(iter, outInput.end());
}

void RemoveWhitespace(String& outInput)
{
	SubstituteChars(outInput, {Pair<char>('\t', ' '), Pair<char>('\n', ' ')});
	RemoveChars(outInput, {' '});
}

Vec<String> Tokenise(const String& s, const char delimiter, InitList<Pair<char>> escape)
{
	auto end = s.cend();
	auto start = end;

	std::stack<Pair<char>> escapeStack;
	Vec<String> v;
	bool escaping = false;
	for (auto it = s.cbegin(); it != end; ++it)
	{
		if (*it != delimiter || escaping)
		{
			if (start == end)
				start = it;
			for (auto e : escape)
			{
				if (*it == e.first)
				{
					escaping = true;
					escapeStack.push(e);
					break;
				}
				else if (*it == e.second)
				{
					if (e.first == escapeStack.top().first)
					{
						escapeStack.pop();
						escaping = !escapeStack.empty();
						break;
					}
				}
			}
			continue;
		}
		if (start != end)
		{
			v.emplace_back(start, it);
			start = end;
		}
	}
	if (start != end)
	{
		v.emplace_back(start, end);
	}
	return v;
}

void SubstituteChars(String& outInput, InitList<Pair<char>> replacements)
{
	String::iterator iter = outInput.begin();
	while (iter != outInput.end())
	{
		for (const auto replacement : replacements)
		{
			if (*iter == replacement.first)
			{
				*iter = replacement.second;
				break;
			}
		}
		++iter;
	}
}

bool IsCharEnclosedIn(const String& str, size_t idx, Strings::Pair<char> wrapper)
{
	size_t idx_1 = idx - 1;
	size_t idx1 = idx + 1;
	return idx < str.length() && idx_1 < str.length() && idx1 < str.length() && idx > 0 &&
		   idx_1 > 0 && idx1 > 0 && str[idx_1] == wrapper.first && str[idx1] == wrapper.second;
}
} // namespace Strings
