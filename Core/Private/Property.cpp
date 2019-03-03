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
}

bool Property::Persistor::Load(const String& filePath)
{
	FileRW fileRW(filePath);
	if (!fileRW.Exists())
	{
		return false;
	}

	String text = fileRW.ReadAll(false);
	Vec<String> lines = Strings::Tokenise(text, '\n', {});
	for (const auto& line : lines)
	{
		if (line.length() <= 0)
			continue;
		if (line.c_str()[0] == '#')
			continue; // Ignore comments

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
	FileRW file(filePath);
	return file.Write(PropertiesToString(properties));
}

Property Property::Persistor::GetProp(const String& key) const
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

void Property::Persistor::SetProp(const Property& property)
{
	for (auto& prop : properties)
	{
		if (prop.key == property.key)
		{
			prop.stringValue = property.stringValue;
			return;
		}
	}
	properties.push_back(property);
}

Property::Property()
{
	key = stringValue = "";
}

Property::Property(const String& key, const String& value)
{
	this->key = key;
	this->stringValue = value;
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
