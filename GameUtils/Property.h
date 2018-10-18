#pragma once
#include <vector>
#include <string>

namespace GameUtils {
	// \brief Maps a string Key to string Value; capable of parsing Value 
	// as int/float. Intention: simple serialisation.
	struct Property {
		// \brief Property persistor; implementation uses a file reader/writer
		class Persistor {
		public:
			bool Load(const std::string& filePath);
			bool Save(const std::string& filePath) const;
			Property GetProp(const std::string& key) const;
			void SetProp(const Property& property);

		private:
			// Vector instead of sets/maps to preserve order of insertion
			std::vector<Property> properties;
		};

		std::string key;
		std::string stringValue;

		Property() : key(""), stringValue("") {}
		Property(const std::string& key, const std::string& value) {
			this->key = key;
			this->stringValue = value;
		}

		int intValue(int defaultValue = -1) const {
			int value = defaultValue;
			try {
				value = std::stoi(stringValue);
			}
			catch (std::exception e) {}
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


}
