#include "GData.h"
#include "Utils.h"

namespace GameUtils {
	GData::GData(const std::string serialised) {
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
			_json = temp.substr(1, temp.size() - 2);
			std::vector<std::string> tokens = Strings::Tokenise(_json, ',');
			for (const auto& token : tokens) {
				std::pair<std::string, std::string> kvp = Strings::Slice(token, ':');
				if (!kvp.second.empty() && !kvp.first.empty()) {
					_values.insert(std::move(kvp));
				}
			}
			return true;
		}
		
		return false;
	}
	
	std::string GData::Unmarshall() {
		return _json;
	}

	void GData::Clear() {
		_values.clear();
		_json = "";
	}

	template<typename T>
	T Get(const hashTable& table, const std::string& key, T (*Adaptor)(const std::string&, T), const T& defaultValue) {
		auto search = table.find(key);
		if (search != table.end()) {
			return Adaptor(search->second, defaultValue);
		}
		return defaultValue;
	}

	std::string GData::GetString(const std::string & key, const std::string & defaultValue) {
		auto search = _values.find(key);
		if (search != _values.end()) {
			return search->second;
		}
		return defaultValue;
	}

	bool GData::GetBool(const std::string & key, bool defaultValue) {
		return Get<bool>(_values, key, &Strings::ToBool, defaultValue);
	}

	int GData::GetInt(const std::string & key, int defaultValue) {
		return Get<int>(_values, key, &Strings::ToInt, defaultValue);
	}

	double GData::GetDouble(const std::string & key, double defaultValue) {
		return Get<double>(_values, key, &Strings::ToDouble, defaultValue);
	}

	GData GData::GetGData(const std::string & key) {
		auto search = _values.find(key);
		if (search != _values.end()) {
			return GData(search->second);
		}
		return GData();
	}

	std::vector<GData> GData::GetVectorGData(const std::string& key) {
		std::vector<GData> ret;
		auto search = _values.find(key);
		if (search != _values.end()) {
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
		auto search = _values.find(key);
		if (search != _values.end()) {
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
