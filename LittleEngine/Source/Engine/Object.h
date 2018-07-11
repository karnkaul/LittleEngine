#pragma once
#include <string>

namespace Game {
	class Object {
	public:
		Object(const std::string& name) : name(name) {}
		virtual ~Object() {}
		const std::string& GetName() const { return name; }
		std::string ToString() const { return "[" + name + "]"; }
	protected:
		std::string name;
	};
}
