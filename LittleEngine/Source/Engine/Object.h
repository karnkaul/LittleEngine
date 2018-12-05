#pragma once
#include <string>

namespace LittleEngine {
	// \brief Base class for ease of logging
	class Object {
	protected:
		std::string m_name;

	public:
		Object(const std::string& name) : m_name(name) {}
		virtual ~Object() {}
		const std::string& GetName() const { return m_name; }
		const std::string GetNameInBrackets() const { return "[" + m_name + "]"; }
		virtual std::string ToString() const { return "[" + m_name + "]"; }
	};
}
