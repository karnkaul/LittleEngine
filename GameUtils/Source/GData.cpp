#include "GData.h"
#include "Utils.h"

namespace GameUtils {
	GData::GData(const std::string& serialised) {
		if (!serialised.empty()) {
			Marshall(serialised);
		}
	}

	bool GData::Marshall(const std::string & serialised) {
		std::string temp(serialised);
		Strings::RemoveWhitespace(temp);
		Strings::RemoveChars(temp, { '"' });
		if (temp[0] == '{' && temp[temp.size() - 1] == '}') {
			Clear();
			_rawText = temp.substr(1, temp.size() - 2);
			std::vector<std::string> tokens = Strings::Tokenise(_rawText, ',');
			for (const auto& token : tokens) {
				std::pair<std::string, std::string> kvp = Strings::Slice(token, ':');
				if (!kvp.second.empty() && !kvp.first.empty()) {
					fieldMap.insert(std::move(kvp));
				}
			}
			return true;
		}
		
		return false;
	}
	
	const std::string& GData::Unmarshall() const {
		return _rawText;
	}

	void GData::Clear() {
		fieldMap.clear();
		_rawText = "";
	}

	template<typename T>
	T Get(const std::unordered_map<std::string, std::string>& table, const std::string& key, T (*Adaptor)(const std::string&, T), const T& defaultValue) {
		auto search = table.find(key);
		if (search != table.end()) {
			return Adaptor(search->second, defaultValue);
		}
		return defaultValue;
	}

	std::string GData::GetString(const std::string & key, const std::string & defaultValue) {
		auto search = fieldMap.find(key);
		if (search != fieldMap.end()) {
			return search->second;
		}
		return defaultValue;
	}

	bool GData::GetBool(const std::string & key, bool defaultValue) {
		return Get<bool>(fieldMap, key, &Strings::ToBool, defaultValue);
	}

	int GData::GetInt(const std::string & key, int defaultValue) {
		return Get<int>(fieldMap, key, &Strings::ToInt, defaultValue);
	}

	double GData::GetDouble(const std::string & key, double defaultValue) {
		return Get<double>(fieldMap, key, &Strings::ToDouble, defaultValue);
	}

	GData GData::GetGData(const std::string & key) {
		auto search = fieldMap.find(key);
		if (search != fieldMap.end()) {
			return GData(search->second);
		}
		return GData();
	}

	std::vector<GData> GData::GetVectorGData(const std::string& key) {
		std::vector<GData> ret;
		auto search = fieldMap.find(key);
		if (search != fieldMap.end()) {
			std::string value = search->second;
			if (value.size() > 2) {
				if (value[0] = '[' && value[value.size() - 1] == ']') {
					value = value.substr(1, value.size() - 2);
					std::vector<std::string> gDatas = Strings::Tokenise(value, ',', { '{', '}', '[', ']', '\"' });
					for (const auto& gData : gDatas) {
						ret.emplace_back(gData);
					}
				}
			}
		} 
		return ret;
	}

	std::vector<std::string> GData::GetVector(const std::string& key) {
		auto search = fieldMap.find(key);
		if (search != fieldMap.end()) {
			std::string value = search->second;
			if (value.size() > 2) {
				if (value[0] = '[' && value[value.size() - 1] == ']') {
					value = value.substr(1, value.size() - 2);
					return Strings::Tokenise(value, ',');
				}
			}
		}
		return std::vector<std::string>();
	}
}
