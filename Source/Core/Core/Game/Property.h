#pragma once
#include <string>
#include "LECoreUtils/StdTypes.h"

namespace Core
{
// \brief Maps a string Key to string Value; capable of parsing Value
// as int/float. Intention: simple serialisation.
struct Property
{
	// \brief Property persistor; implementation uses a file reader/writer
	class Persistor
	{
	public:
		void Deserialise(std::string serialised);
		bool Load(const std::string& filePath);
		bool Save(const std::string& filePath) const;
		const Property* GetProp(const std::string& key) const;
		void SetProp(Property property);

	private:
		// Vector instead of sets/maps to preserve order of insertion
		std::vector<Property> properties;
	};

	std::string key;
	std::string stringValue;

	Property();
	Property(std::string key, std::string value);

	s32 ToS32(s32 defaultValue = -1) const;
	f32 ToF32(f32 defaultValue = -1) const;
	bool ToBool(bool defaultValue = false) const;

	operator std::string() const;
};
} // namespace Core
