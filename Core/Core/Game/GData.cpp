#include "GData.h"
#include "GameTypes/Utils.h"

namespace Core
{
namespace
{
std::initializer_list<std::pair<char, char>> gDataEscapes = {{'{', '}'}, {'[', ']'}, {'"', '"'}};

template <typename T>
T Get(const std::unordered_map<std::string, std::string>& table, const std::string& key, T (*Adaptor)(std::string, T),
	  const T& defaultValue)
{
	auto search = table.find(key);
	if (search != table.end())
	{
		return Adaptor(search->second, defaultValue);
	}
	return defaultValue;
}
} // namespace

GData::GData(std::string serialised)
{
	if (!serialised.empty())
	{
		Marshall(std::move(serialised));
	}
}

GData::GData() = default;
GData::~GData() = default;

bool GData::Marshall(std::string serialised)
{
	Strings::RemoveChars(serialised, {'\t', '\r', '\n'});
	Strings::Trim(serialised, {' '});
	if (serialised[0] == '{' && serialised[serialised.size() - 1] == '}')
	{
		Clear();
		std::string rawText = serialised.substr(1, serialised.size() - 2);
		std::vector<std::string> tokens = Strings::Tokenise(rawText, ',', gDataEscapes);
		for (const auto& token : tokens)
		{
			std::pair<std::string, std::string> kvp = Strings::Bisect(token, ':');
			if (!kvp.second.empty() && !kvp.first.empty())
			{
				std::initializer_list<char> trim = {' ', '"'};
				Strings::Trim(kvp.first, trim);
				Strings::Trim(kvp.second, trim);
				m_fieldMap.emplace(std::move(kvp.first), std::move(kvp.second));
			}
		}
		return true;
	}

	return false;
}

std::string GData::Unmarshall() const
{
	std::string ret = "{";
	size_t slice = 0;
	for (const auto& kvp : m_fieldMap)
	{
		std::string value = kvp.second;
		Strings::Trim(value, {' '});
		auto space = value.find(' ');
		if (Strings::IsCharEnclosedIn(value, space, {'"', '"'}))
		{
			value = '\"' + value + '\"';
		}
		ret += (kvp.first + ':' + value + ',');
		slice = 1;
	}
	return ret.substr(0, ret.size() - slice) + '}';
}

void GData::Clear()
{
	m_fieldMap.clear();
}

std::string GData::GetString(const std::string& key, std::string defaultValue) const
{
	auto search = m_fieldMap.find(key);
	if (search != m_fieldMap.end())
	{
		return search->second;
	}
	return defaultValue;
}

std::string GData::GetString(const std::string& key, char spaceDelimiter, std::string defaultValue) const
{
	std::string ret = std::move(defaultValue);
	auto search = m_fieldMap.find(key);
	if (search != m_fieldMap.end())
	{
		ret = search->second;
		Strings::SubstituteChars(ret, {std::pair<char, char>(spaceDelimiter, ' ')});
	}
	return ret;
}

bool GData::GetBool(const std::string& key, bool defaultValue) const
{
	return Get<bool>(m_fieldMap, key, &Strings::ToBool, defaultValue);
}

s32 GData::GetS32(const std::string& key, s32 defaultValue) const
{
	return Get<s32>(m_fieldMap, key, &Strings::ToS32, defaultValue);
}

f64 GData::GetF64(const std::string& key, f64 defaultValue) const
{
	return Get<f64>(m_fieldMap, key, &Strings::ToF64, defaultValue);
}

Vector2 GData::GetVector2(const std::string& key, Vector2 defaultValue) const
{
	auto search = m_fieldMap.find(key);
	if (search != m_fieldMap.end())
	{
		GData vec2Data(search->second);
		return Vector2(Fixed(vec2Data.GetF64("x", defaultValue.x.ToF64())), Fixed(vec2Data.GetF64("y", defaultValue.y.ToF64())));
	}
	return defaultValue;
}

GData GData::GetGData(const std::string& key) const
{
	auto search = m_fieldMap.find(key);
	if (search != m_fieldMap.end())
	{
		return GData(search->second);
	}
	return {};
}

std::vector<GData> GData::GetVectorGData(const std::string& key) const
{
	std::vector<GData> ret;
	std::vector<std::string> rawStrings = GetVector(key);
	for (auto& rawString : rawStrings)
	{
		Strings::Trim(rawString, {'"', ' '});
		ret.emplace_back(std::move(rawString));
	}
	return ret;
}

std::vector<std::string> GData::GetVector(const std::string& key) const
{
	auto search = m_fieldMap.find(key);
	if (search != m_fieldMap.end())
	{
		std::string value = search->second;
		if (value.size() > 2)
		{
			Strings::Trim(value, {' '});
			if (value[0] == '[' && value[value.size() - 1] == ']')
			{
				value = value.substr(1, value.size() - 2);
				Strings::Trim(value, {' '});
				std::vector<std::string> ret = Strings::Tokenise(value, ',', gDataEscapes);
				for (auto& str : ret)
				{
					Strings::Trim(str, {'"', ' '});
				}
				return ret;
			}
		}
	}
	return std::vector<std::string>();
}

const std::unordered_map<std::string, std::string>& GData::AllFields() const
{
	return m_fieldMap;
}

bool GData::AddField(std::string key, GData& gData)
{
	std::string value = gData.Unmarshall();
	Strings::RemoveChars(value, {'\"'});
	return SetString(std::move(key), std::move(value));
}

bool GData::SetString(std::string key, std::string value)
{
	if (!key.empty() && !value.empty())
	{
		m_fieldMap[std::move(key)] = std::move(value);
		return true;
	}
	return false;
}

u32 GData::NumFields() const
{
	return static_cast<u32>(m_fieldMap.size());
}

bool GData::Contains(const std::string& id) const
{
	return m_fieldMap.find(id) != m_fieldMap.end();
}
} // namespace Core
