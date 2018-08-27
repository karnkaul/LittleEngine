#pragma once
#include <vector>
#include <string>

struct Property {
	std::string key;
	std::string stringValue;

	Property() { key = stringValue = ""; }
	Property(const std::string& key, const std::string& value) {
		this->key = key;
		this->stringValue = value;
	}

	int intValue(int defaultValue = -1) const {
		int value = defaultValue;
		try {
			value = std::stoi(stringValue);
		}
		catch (std::exception e) { }
		return value;
	}

	float floatValue(float defaultValue = -1) const {
		float value = defaultValue;
		try {
			value = std::stof(stringValue);
		}
		catch (std::exception e) {}
		return value;
	}

	operator std::string() const {
		return stringValue;
	}
};

class PropRW {
public:
	bool Load(const std::string& filePath);
	bool Save(const std::string& filePath) const;
	Property GetProp(const std::string& key) const;
	void SetProp(const Property& property);
private:
	std::vector<Property> properties;
};


