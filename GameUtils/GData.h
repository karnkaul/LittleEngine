#pragma once
#include <unordered_map>
#include "Fixed.h"

namespace GameUtils {
	// \brief Pseudo-JSON serialisable data container
	class GData {
	private:
		std::unordered_map<std::string, std::string> m_fieldMap;

	public:
		GData() = default;
		// Pass serialised data to marhshall and load fields
		GData(const std::string& serialised);
		GData(const GData& rhs) = default;
		GData(GData&&) = default;
		GData& operator=(const GData&) = default;
		GData& operator=(GData&&) = default;

		// Marhshalls and load fields from serialised data
		bool Marshall(const std::string& serialised);
		// Returns original raw data, without whitespaces and enclosing braces
		std::string Unmarshall() const;
		// Clears raw data and fields
		void Clear();

		std::string GetString(const std::string& key, const std::string& defaultValue = "") const;
		bool GetBool(const std::string& key, bool defaultValue = false) const;
		int GetInt(const std::string& key, int defaultValue = -1) const;
		double GetDouble(const std::string& key, double defaultValue = -1.0f) const;
		GData GetGData(const std::string& key) const;

		std::vector<GData> GetVectorGData(const std::string& key) const;
		std::vector<std::string> GetVector(const std::string& key) const;

		bool AddField(const std::string& key, GData& gData);
		bool SetString(const std::string& key, const std::string& value);

		int NumFields() const;
	};
}
