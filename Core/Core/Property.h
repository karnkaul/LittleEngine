#pragma once
#include "CoreTypes.h"

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
		bool Load(String filePath);
		bool Save(String filePath) const;
		const Property* GetProp(String key) const;
		void SetProp(Property property);

	private:
		// Vector instead of sets/maps to preserve order of insertion
		Vec<Property> properties;
	};

	String key;
	String stringValue;

	Property();
	Property(String key, String value);
	Property(Property&&) = default;
	Property& operator=(Property&&) = default;
	Property(const Property&) = default;
	Property& operator=(const Property&) = default;

	s32 ToS32(s32 defaultValue = -1) const;
	f32 ToF32(f32 defaultValue = -1) const;

	operator String() const;
};

} // namespace Core
