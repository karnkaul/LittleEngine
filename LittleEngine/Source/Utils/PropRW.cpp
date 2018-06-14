#include "stdafx.h"
#include <vector>
#include "PropRW.h"
#include "FileRW.h"

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
	if (start != end)
		v.emplace_back(start, end);
	return v;
}

std::string PropertiesToString(const std::map<std::string, Property>& map) {
	auto iter = map.begin();
	std::string fileContents;
	while (iter != map.end()) {
		std::string value = iter->second;
		fileContents += (iter->first + ":" + value + "\n");
		++iter;
	}
	return fileContents;
}

bool PropRW::Load(const std::string& filePath) {
	FileRW fileRW(filePath);
	if (!fileRW.Exists()) {
		return false;
	}

	std::vector<std::string> lines = fileRW.Read();
	for (const auto& line : lines) {
		if (line.length() <= 0) continue;
		if (line.c_str()[0] == '#') continue;

		std::vector<std::string> tokens = Tokenize(line, ':');
		if (tokens.size() > 1) {
			SetProp(Property(tokens[0], tokens[1]));
		}
	}
	return true;
}

bool PropRW::Save(const std::string& filePath) const {
		FileRW file(filePath);
	return file.Write(PropertiesToString(properties));
}

Property PropRW::GetProp(const std::string& key) const {
	auto search = properties.find(key);
	if (search != properties.end()) {
		return Property(search->second);
	}
	return Property();
}

void PropRW::SetProp(const Property& property) {
	auto search = properties.find(property.key);
	if (search != properties.end()) {
		search->second.stringValue = property.stringValue;
	}
	else {
		properties.insert(std::pair<std::string, Property>(property.key, property));
	}
}
