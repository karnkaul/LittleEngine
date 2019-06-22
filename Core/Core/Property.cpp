#include "stdafx.h"
#include <vector>
#include "Property.h"
#include "FileRW.h"
#include "Utils.h"

namespace Core
{
namespace
{
String PropertiesToString(const Vec<Property>& vec)
{
	String fileContents;
	for (const auto& prop : vec)
	{
		fileContents += (prop.key + "=" + prop.stringValue + "\n");
	}
	return fileContents;
}
} // namespace

bool Property::Persistor::Load(const String& filePath)
{
	FileRW fileRW(std::move(filePath));
	if (!fileRW.Exists())
	{
		return false;
	}

	String text = fileRW.ReadAll(false);
	Vec<String> lines = Strings::Tokenise(text, '\n', {});
	for (const auto& line : lines)
	{
		if (line.empty() || line.c_str()[0] == '#')
		{
			continue;
		}

		Vec<String> tokens = Strings::Tokenise(line, '=', {});
		if (tokens.size() > 1)
		{
			SetProp(Property(tokens[0], tokens[1]));
		}
	}
	return true;
}

bool Property::Persistor::Save(const String& filePath) const
{
	FileRW file(std::move(filePath));
	return file.Write(PropertiesToString(properties));
}

const Property* Property::Persistor::GetProp(const String& key) const
{
	auto iter = std::find_if(properties.begin(), properties.end(),
							 [&](const Property& p) { return p.key == key; });
	return iter != properties.end() ? &(*iter) : nullptr;
}

void Property::Persistor::SetProp(Property property)
{
	auto iter = std::find_if(properties.begin(), properties.end(),
							 [&](const Property& p) { return p.key == property.key; });
	if (iter != properties.end())
	{
		iter->stringValue = std::move(property.stringValue);
	}
	else
	{
		properties.emplace_back(std::move(property));
	}
}

Property::Property() = default;

Property::Property(String key, String value) : key(std::move(key)), stringValue(std::move(value))
{
}

s32 Property::ToS32(s32 defaultValue) const
{
	return Strings::ToS32(stringValue, defaultValue);
}

f32 Property::ToF32(f32 defaultValue) const
{
	return Strings::ToF32(stringValue, defaultValue);
}

bool Property::ToBool(bool defaultValue /* = false */) const
{
	return Strings::ToBool(stringValue, defaultValue);
}

Property::operator String() const
{
	return stringValue;
}
} // namespace Core
