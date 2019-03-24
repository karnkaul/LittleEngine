#pragma once
#include "StdTypes.h"

namespace Core
{
// \brief Pseudo-JSON serialisable data container
class GData
{
private:
	UMap<String, String> m_fieldMap;

public:
	GData() = default;
	// Pass serialised data to marhshall and load fields
	GData(const String& serialised);
	GData(const GData& rhs) = default;
	GData(GData&&) = default;
	GData& operator=(const GData&) = default;
	GData& operator=(GData&&) = default;

	// Marhshalls and load fields from serialised data
	bool Marshall(const String& serialised);
	// Returns original raw data, without whitespaces and enclosing braces
	String Unmarshall() const;
	// Clears raw data and fields
	void Clear();

	String GetString(const String& key, const String& defaultValue = "") const;
	String GetString(const String& key, const char spaceDelimiter, const String& defaultValue) const;
	bool GetBool(const String& key, bool defaultValue = false) const;
	s32 GetS32(const String& key, s32 defaultValue = -1) const;
	f64 GetF64(const String& key, f64 defaultValue = -1.0) const;
	GData GetGData(const String& key) const;

	Vec<GData> GetVectorGData(const String& key) const;
	Vec<String> GetVector(const String& key) const;

	bool AddField(const String& key, GData& gData);
	bool SetString(const String& key, const String& value);

	u32 NumFields() const;
};
} // namespace Core
