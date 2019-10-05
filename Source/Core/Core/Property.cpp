#include <vector>
#include "Property.h"
#include "FileRW.h"
#include "Utils.h"

namespace Core
{
namespace
{
std::string PropertiesToString(const std::vector<Property>& vec)
{
	std::string fileContents;
	for (const auto& prop : vec)
	{
		fileContents.append(prop.key).append("=").append(prop.stringValue).append("\n");
	}
	return fileContents;
}
} // namespace

void Property::Persistor::Deserialise(std::string serialised)
{
	std::vector<std::string> lines = Strings::Tokenise(serialised, '\n', {});
	for (const auto& line : lines)
	{
		if (line.empty() || line.c_str()[0] == '#')
		{
			continue;
		}

		std::vector<std::string> tokens = Strings::Tokenise(line, '=', {});
		if (tokens.size() > 1)
		{
			SetProp(Property(tokens[0], tokens[1]));
		}
	}
}

bool Property::Persistor::Load(const std::string& filePath)
{
	FileRW fileRW(std::move(filePath));
	if (!fileRW.Exists())
	{
		return false;
	}

	Deserialise(fileRW.Load(false));
	return true;
}

bool Property::Persistor::Save(const std::string& filePath) const
{
	FileRW file(std::move(filePath));
	return file.Write(PropertiesToString(properties));
}

const Property* Property::Persistor::GetProp(const std::string& key) const
{
	auto iter = std::find_if(properties.begin(), properties.end(), [&](const Property& p) { return p.key == key; });
	return iter != properties.end() ? &(*iter) : nullptr;
}

void Property::Persistor::SetProp(Property property)
{
	auto iter = std::find_if(properties.begin(), properties.end(), [&](const Property& p) { return p.key == property.key; });
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

Property::Property(std::string key, std::string value) : key(std::move(key)), stringValue(std::move(value)) {}

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

Property::operator std::string() const
{
	return stringValue;
}
} // namespace Core
