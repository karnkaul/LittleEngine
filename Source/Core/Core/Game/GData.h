#pragma once
#include <unordered_map>
#include <vector>
#include "GameTypes/StdTypes.h"
#include "GameTypes/Vector2.h"

namespace Core
{
// \brief Pseudo-JSON serialisable data container
class GData
{
private:
	std::unordered_map<std::string, std::string> m_fieldMap;

public:
	GData();
	// Pass serialised data to marhshall and load fields
	GData(std::string serialised);
	GData(const GData& rhs) = default;
	GData(GData&&) = default;
	GData& operator=(const GData&) = default;
	GData& operator=(GData&&) = default;
	~GData();

	// Marhshalls and load fields from serialised data
	bool Marshall(std::string serialised);
	// Returns original raw data, without whitespaces and enclosing braces
	std::string Unmarshall() const;
	// Clears raw data and fields
	void Clear();

	std::string GetString(const std::string& key, std::string defaultValue = "") const;
	std::string GetString(const std::string& key, char spaceDelimiter, std::string defaultValue) const;
	bool GetBool(const std::string& key, bool defaultValue = false) const;
	s32 GetS32(const std::string& key, s32 defaultValue = -1) const;
	f64 GetF64(const std::string& key, f64 defaultValue = -1.0) const;
	Vector2 GetVector2(const std::string& key, Vector2 defaultValue = {-1, -1}) const;
	GData GetGData(const std::string& key) const;

	std::vector<GData> GetVectorGData(const std::string& key) const;
	std::vector<std::string> GetVector(const std::string& key) const;

	const std::unordered_map<std::string, std::string>& AllFields() const;
	bool AddField(std::string key, GData& gData);
	bool SetString(std::string key, std::string value);

	u32 NumFields() const;
	bool Contains(const std::string& id) const;
};
} // namespace Core
