#include "Utils.h"
#include <algorithm>
#include <stack>

namespace Core
{
Pair<f32, VString> FriendlySize(u64 byteCount)
{
	static Array<VString, 4> suffixes = {"B", "KiB", "MiB", "GiB"};
	f32 bytes = static_cast<f32>(byteCount);
	size_t idx = 0;
	while (bytes > 1024.0f && idx < 4)
	{
		++idx;
		bytes /= 1024.0f;
	}
	return std::make_pair(bytes, suffixes[idx < 4 ? idx : 3]);
}
} // namespace Core

namespace Strings
{
void ToLower(String& outString)
{
	std::transform(outString.begin(), outString.end(), outString.begin(), ::tolower);
}

void ToUpper(String& outString)
{
	std::transform(outString.begin(), outString.end(), outString.begin(), ::toupper);
}

bool ToBool(String input, bool bDefaultValue)
{
	if (!input.empty())
	{
		ToLower(input);
		if (input == "true" || input == "1")
		{
			return true;
		}
		if (input == "false" || input == "0")
		{
			return false;
		}
	}
	return bDefaultValue;
}

s32 ToS32(String input, s32 defaultValue)
{
	s32 ret = defaultValue;
	if (!input.empty())
	{
		try
		{
			ret = std::stoi(input);
		}
		catch (const std::invalid_argument&)
		{
			ret = defaultValue;
		}
	}
	return ret;
}

f32 ToF32(String input, f32 defaultValue)
{
	f32 ret = defaultValue;
	if (!input.empty())
	{
		try
		{
			ret = std::stof(input);
		}
		catch (const std::exception&)
		{
			ret = defaultValue;
		}
	}
	return ret;
}

f64 ToF64(String input, f64 defaultValue)
{
	f64 ret = defaultValue;
	if (!input.empty())
	{
		try
		{
			ret = std::stod(input);
		}
		catch (const std::exception&)
		{
			ret = defaultValue;
		}
	}
	return ret;
}

String ToString(bool bInput)
{
	return bInput ? "true" : "false";
}

String ToText(Vec<u8> rawBuffer)
{
	Vec<char> charBuffer(rawBuffer.size() + 1, 0);
	for (size_t i = 0; i < rawBuffer.size(); ++i)
	{
		charBuffer[i] = static_cast<char>(rawBuffer[i]);
	}
	return String(charBuffer.data());
}

Dual<String> Bisect(VString input, char delimiter)
{
	size_t idx = input.find(delimiter);
	return idx < input.size() ? Dual<String>(input.substr(0, idx), input.substr(idx + 1, input.size())) : Dual<String>(String(input), {});
}

void RemoveChars(String& outInput, InitList<char> toRemove)
{
	auto isToRemove = [&toRemove](char c) -> bool { return std::find(toRemove.begin(), toRemove.end(), c) != toRemove.end(); };
	auto iter = std::remove_if(outInput.begin(), outInput.end(), isToRemove);
	outInput.erase(iter, outInput.end());
}

void Trim(String& outInput, InitList<char> toRemove)
{
	auto isIgnored = [&outInput, &toRemove](size_t idx) {
		return std::find(toRemove.begin(), toRemove.end(), outInput[idx]) != toRemove.end();
	};
	size_t startIdx = 0;
	for (; startIdx < outInput.size() && isIgnored(startIdx); ++startIdx)
		;
	size_t endIdx = outInput.size();
	for (; endIdx > startIdx && isIgnored(endIdx - 1); --endIdx)
		;
	outInput = outInput.substr(startIdx, endIdx - startIdx);
}

void RemoveWhitespace(String& outInput)
{
	SubstituteChars(outInput, {Dual<char>('\t', ' '), Dual<char>('\n', ' '), Dual<char>('\r', ' ')});
	RemoveChars(outInput, {' '});
}

Vec<String> Tokenise(VString s, char delimiter, InitList<Dual<char>> escape)
{
	auto end = s.cend();
	auto start = end;

	std::stack<Dual<char>> escapeStack;
	Vec<String> v;
	bool escaping = false;
	for (auto it = s.cbegin(); it != end; ++it)
	{
		if (*it != delimiter || escaping)
		{
			if (start == end)
			{
				start = it;
			}
			for (auto e : escape)
			{
				if (escaping && *it == e.second)
				{
					if (e.first == escapeStack.top().first)
					{
						escapeStack.pop();
						escaping = !escapeStack.empty();
						break;
					}
				}
				if (*it == e.first)
				{
					escaping = true;
					escapeStack.push(e);
					break;
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

void SubstituteChars(String& outInput, InitList<Dual<char>> replacements)
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

bool IsCharEnclosedIn(VString str, size_t idx, Dual<char> wrapper)
{
	size_t idx_1 = idx - 1;
	size_t idx1 = idx + 1;
	return idx1 < str.length() && str[idx_1] == wrapper.first && str[idx1] == wrapper.second;
}
} // namespace Strings
