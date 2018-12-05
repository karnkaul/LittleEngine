#include <vector>
#include "Property.h"
#include "FileRW.h"

namespace GameUtils {
	std::vector<std::string> Tokenize(const std::string& s, char c) {
		auto end = s.cend();
		auto start = end;

		std::vector<std::string> v;
		for (auto it = s.cbegin(); it != end; ++it) {
			if (*it != c) {
				if (start == end)
					start = it;
				continue;
			}
			if (start != end) {
				v.emplace_back(start, it);
				start = end;
			}
		}
		if (start != end) {
			v.emplace_back(start, end);
		}
		return v;
	}

	std::string PropertiesToString(const std::vector<Property>& vec) {
		std::string fileContents;
		for (const auto& prop : vec) {
			fileContents += (prop.key + ":" + prop.stringValue + "\n");
		}
		return fileContents;
	}


	bool Property::Persistor::Load(const std::string& filePath) {
		FileRW fileRW(filePath);
		if (!fileRW.Exists()) {
			return false;
		}

		const std::vector<std::string>& lines = fileRW.ReadLines();
		for (const auto& line : lines) {
			if (line.length() <= 0) continue;
			if (line.c_str()[0] == '#') continue;	// Ignore comments

			std::vector<std::string> tokens = Tokenize(line, ':');
			if (tokens.size() > 1) {
				SetProp(Property(tokens[0], tokens[1]));
			}
		}
		return true;
	}

	bool Property::Persistor::Save(const std::string& filePath) const {
		FileRW file(filePath);
		return file.Write(PropertiesToString(m_properties));
	}

	Property Property::Persistor::GetProp(const std::string& key) const {
		for (auto& prop : m_properties) {
			if (prop.key == key) {
				return prop;
			}
		}
		return Property();
	}

	void Property::Persistor::SetProp(const Property& property) {
		for (auto& prop : m_properties) {
			if (prop.key == property.key) {
				prop.stringValue = property.stringValue;
				return;
			}
		}
		m_properties.push_back(property);
	}
}
