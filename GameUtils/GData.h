#pragma once
#include <unordered_map>
#include "Fixed.h"

namespace GameUtils {
	using hashTable = std::unordered_map<std::string, std::string>;
	
	class GData {
	public:
		GData() = default;
		GData(const std::string serialised);
		GData(const GData& rhs) = default;
		GData(GData&&) = default;
		GData& operator=(const GData&) = default;
		GData& operator=(GData&&) = default;

		bool Marshall(const std::string& serialised);
		std::string Unmarshall();
		void Clear();

		std::string GetString(const std::string& key, const std::string& defaultValue = "");
		bool GetBool(const std::string& key, bool defaultValue = false);
		int GetInt(const std::string& key, int defaultValue = -1);
		double GetDouble(const std::string& key, double defaultValue = -1.0f);
		GData GetGData(const std::string& key);

		std::vector<GData> GetVectorGData(const std::string& key);
		std::vector<std::string> GetVector(const std::string& key);

	private:
		std::string _json;
		hashTable _values;
	};
}
