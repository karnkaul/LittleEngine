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
		bool Load(const String& filePath);
		bool Save(const String& filePath) const;
		Property GetProp(const String& key) const;
		void SetProp(const Property& property);

	private:
		// Vector instead of sets/maps to preserve order of insertion
		Vector<Property> properties;
	};

	String key;
	String stringValue;

	Property();
	Property(const String& key, const String& value);

	s32 ToS32(s32 defaultValue = -1) const;
	f32 ToF32(f32 defaultValue = -1) const;

	operator String() const;
};

} // namespace Core
