#include "GData.h"
#include "Utils.h"

namespace GameUtils {
	namespace {
		std::initializer_list<Strings::Pair<char>> gDataEscapes = {
			{ '{', '}' },
			{ '[', ']' },
			'\"'
		};
	}

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
			std::string rawText = temp.substr(1, temp.size() - 2);
			std::vector<std::string> tokens = Strings::Tokenise(rawText, ',', gDataEscapes);
			for (const auto& token : tokens) {
				Strings::Pair<std::string> kvp = Strings::Bisect(token, ':');
				if (!kvp.second.empty() && !kvp.first.empty()) {
					m_fieldMap.emplace(kvp.first, kvp.second);
				}
			}
			return true;
		}
		
		return false;
	}
	
	std::string GData::Unmarshall() const {
		std::string ret = "{";
		size_t slice = 0;
		for (const auto& kvp : m_fieldMap) {
			std::string value = kvp.second;
			auto space = value.find(' ');
			if (Strings::IsCharEnclosedIn(value, space, '\"')) value = '\"' + value + '\"';
			ret += (kvp.first + ':' + value + ',');
			slice = 1;
		}
		return ret.substr(0, ret.size() - slice) + '}';
	}

	void GData::Clear() {
		m_fieldMap.clear();
	}

	template<typename T>
	static T Get(const std::unordered_map<std::string, std::string>& table, const std::string& key, T (*Adaptor)(const std::string&, T), const T& defaultValue) {
		auto search = table.find(key);
		if (search != table.end()) {
			return Adaptor(search->second, defaultValue);
		}
		return defaultValue;
	}

	std::string GData::GetString(const std::string & key, const std::string & defaultValue) const {
		auto search = m_fieldMap.find(key);
		if (search != m_fieldMap.end()) {
			return search->second;
		}
		return defaultValue;
	}

	bool GData::GetBool(const std::string & key, bool defaultValue) const {
		return Get<bool>(m_fieldMap, key, &Strings::ToBool, defaultValue);
	}

	int GData::GetInt(const std::string & key, int defaultValue) const {
		return Get<int>(m_fieldMap, key, &Strings::ToInt, defaultValue);
	}

	double GData::GetDouble(const std::string & key, double defaultValue) const {
		return Get<double>(m_fieldMap, key, &Strings::ToDouble, defaultValue);
	}

	GData GData::GetGData(const std::string & key) const {
		auto search = m_fieldMap.find(key);
		if (search != m_fieldMap.end()) {
			return GData(search->second);
		}
		return GData();
	}

	std::vector<GData> GData::GetVectorGData(const std::string& key) const {
		std::vector<GData> ret;
		auto search = m_fieldMap.find(key);
		if (search != m_fieldMap.end()) {
			std::string value = search->second;
			if (value.size() > 2) {
				if (value[0] == '[' && value[value.size() - 1] == ']') {
					value = value.substr(1, value.size() - 2);
					std::vector<std::string> gDatas = Strings::Tokenise(value, ',', gDataEscapes);
					for (const auto& gData : gDatas) {
						ret.emplace_back(gData);
					}
				}
			}
		} 
		return ret;
	}

	std::vector<std::string> GData::GetVector(const std::string& key) const {
		auto search = m_fieldMap.find(key);
		if (search != m_fieldMap.end()) {
			std::string value = search->second;
			if (value.size() > 2) {
				if (value[0] == '[' && value[value.size() - 1] == ']') {
					value = value.substr(1, value.size() - 2);
					return Strings::Tokenise(value, ',', { '\"' });
				}
			}
		}
		return std::vector<std::string>();
	}

	bool GData::AddField(const std::string& key, GData & gData) {
		std::string value = gData.Unmarshall();
		Strings::RemoveChars(value, { '\"' });
		return SetString(key, value);
	}
	
	bool GData::SetString(const std::string & key, const std::string & value) {
		if (!key.empty() && !value.empty()) {
			m_fieldMap[key] = value;
			return true;
		}
		return false;
	}

	int GData::NumFields() const {
		return static_cast<int>(m_fieldMap.size());
	}
}
