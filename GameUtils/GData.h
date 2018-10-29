#pragma once
#include <unordered_map>
#include "Fixed.h"

namespace GameUtils {
	// \brief Pseudo-JSON serialisable data container
	class GData {
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
		const std::string& Unmarshall() const;
		// Clears raw data and fields
		void Clear();

		std::string GetString(const std::string& key, const std::string& defaultValue = "");
		bool GetBool(const std::string& key, bool defaultValue = false);
		int GetInt(const std::string& key, int defaultValue = -1);
		double GetDouble(const std::string& key, double defaultValue = -1.0f);
		GData GetGData(const std::string& key);

		std::vector<GData> GetVectorGData(const std::string& key);
		std::vector<std::string> GetVector(const std::string& key);

	private:
		std::string _rawText;
		std::unordered_map<std::string, std::string> fieldMap;
	};
}
