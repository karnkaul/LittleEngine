#pragma once
#include <string>

namespace Game {
	// \brief Base class for ease of logging
	class Object {
	public:
		Object(const std::string& name) : name(name) {}
		virtual ~Object() {}
		const std::string& GetName() const { return name; }
		virtual std::string ToString() const { return "[" + name + "]"; }
	protected:
		std::string name;
	};
}
