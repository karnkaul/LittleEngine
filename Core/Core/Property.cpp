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

bool Property::Persistor::Load(String filePath)
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
		if (line.length() <= 0 || line.c_str()[0] == '#')
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

bool Property::Persistor::Save(String filePath) const
{
	FileRW file(std::move(filePath));
	return file.Write(PropertiesToString(properties));
}

Property Property::Persistor::GetProp(String key) const
{
	for (auto& prop : properties)
	{
		if (prop.key == key)
		{
			return prop;
		}
	}
	return Property();
}

void Property::Persistor::SetProp(Property property)
{
	for (auto& prop : properties)
	{
		if (prop.key == property.key)
		{
			prop.stringValue = property.stringValue;
			return;
		}
	}
	properties.emplace_back(std::move(property));
}

Property::Property() = default;

Property::Property(String key, String value) : key(std::move(key)), stringValue(std::move(value))
{
}

s32 Property::ToS32(s32 defaultValue) const
{
	s32 value = defaultValue;
	try
	{
		value = std::stoi(stringValue);
	}
	catch (const std::exception&)
	{
	}
	return value;
}

f32 Property::ToF32(f32 defaultValue) const
{
	f32 value = defaultValue;
	try
	{
		value = std::stof(stringValue);
	}
	catch (const std::exception&)
	{
	}
	return value;
}

Property::operator String() const
{
	return stringValue;
}
} // namespace Core
