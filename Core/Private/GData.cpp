#include "stdafx.h"
#include "GData.h"
#include "Utils.h"

namespace Core
{
namespace
{
InitList<Strings::Pair<char>> gDataEscapes = {{'{', '}'}, {'[', ']'}, '\"'};
}

GData::GData(const String& serialised)
{
	if (!serialised.empty())
	{
		Marshall(serialised);
	}
}

bool GData::Marshall(const String& serialised)
{
	String temp(serialised);
	Strings::RemoveWhitespace(temp);
	Strings::RemoveChars(temp, {'"'});
	if (temp[0] == '{' && temp[temp.size() - 1] == '}')
	{
		Clear();
		String rawText = temp.substr(1, temp.size() - 2);
		Vec<String> tokens = Strings::Tokenise(rawText, ',', gDataEscapes);
		for (const auto& token : tokens)
		{
			Strings::Pair<String> kvp = Strings::Bisect(token, ':');
			if (!kvp.second.empty() && !kvp.first.empty())
			{
				m_fieldMap.emplace(kvp.first, kvp.second);
			}
		}
		return true;
	}

	return false;
}

String GData::Unmarshall() const
{
	String ret = "{";
	size_t slice = 0;
	for (const auto& kvp : m_fieldMap)
	{
		String value = kvp.second;
		auto space = value.find(' ');
		if (Strings::IsCharEnclosedIn(value, space, '\"'))
			value = '\"' + value + '\"';
		ret += (kvp.first + ':' + value + ',');
		slice = 1;
	}
	return ret.substr(0, ret.size() - slice) + '}';
}

void GData::Clear()
{
	m_fieldMap.clear();
}

template <typename T>
static T Get(const UMap<String, String>& table, const String& key, T (*Adaptor)(const String&, T), const T& defaultValue)
{
	auto search = table.find(key);
	if (search != table.end())
	{
		return Adaptor(search->second, defaultValue);
	}
	return defaultValue;
}

String GData::GetString(const String& key, const String& defaultValue) const
{
	auto search = m_fieldMap.find(key);
	if (search != m_fieldMap.end())
	{
		return search->second;
	}
	return defaultValue;
}

String GData::GetString(const String& key, const char spaceDelimiter, const String& defaultValue) const
{
	String ret = defaultValue;
	auto search = m_fieldMap.find(key);
	if (search != m_fieldMap.end())
	{
		ret = search->second;
		Strings::SubstituteChars(ret, {Strings::Pair<char>(spaceDelimiter, ' ')});
	}
	return ret;
}

bool GData::GetBool(const String& key, bool defaultValue) const
{
	return Get<bool>(m_fieldMap, key, &Strings::ToBool, defaultValue);
}

s32 GData::GetS32(const String& key, s32 defaultValue) const
{
	return Get<s32>(m_fieldMap, key, &Strings::ToS32, defaultValue);
}

f64 GData::GetF64(const String& key, f64 defaultValue) const
{
	return Get<f64>(m_fieldMap, key, &Strings::ToF64, defaultValue);
}

GData GData::GetGData(const String& key) const
{
	auto search = m_fieldMap.find(key);
	if (search != m_fieldMap.end())
	{
		return GData(search->second);
	}
	return GData();
}

Vec<GData> GData::GetVectorGData(const String& key) const
{
	Vec<GData> ret;
	auto search = m_fieldMap.find(key);
	if (search != m_fieldMap.end())
	{
		String value = search->second;
		if (value.size() > 2)
		{
			if (value[0] == '[' && value[value.size() - 1] == ']')
			{
				value = value.substr(1, value.size() - 2);
				Vec<String> gDatas = Strings::Tokenise(value, ',', gDataEscapes);
				for (const auto& gData : gDatas)
				{
					ret.emplace_back(gData);
				}
			}
		}
	}
	return ret;
}

Vec<String> GData::GetVector(const String& key) const
{
	auto search = m_fieldMap.find(key);
	if (search != m_fieldMap.end())
	{
		String value = search->second;
		if (value.size() > 2)
		{
			if (value[0] == '[' && value[value.size() - 1] == ']')
			{
				value = value.substr(1, value.size() - 2);
				return Strings::Tokenise(value, ',', {'\"'});
			}
		}
	}
	return Vec<String>();
}

bool GData::AddField(const String& key, GData& gData)
{
	String value = gData.Unmarshall();
	Strings::RemoveChars(value, {'\"'});
	return SetString(key, value);
}

bool GData::SetString(const String& key, const String& value)
{
	if (!key.empty() && !value.empty())
	{
		m_fieldMap[key] = value;
		return true;
	}
	return false;
}

u32 GData::NumFields() const
{
	return static_cast<s32>(m_fieldMap.size());
}
} // namespace Core
