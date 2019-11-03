#pragma once
#include <bitset>
#include "SFML/Graphics.hpp"
#include "Core/CoreTypes.h"

namespace LittleEngine
{
class Shader : private NoCopy
{
public:
	enum class Flag : u8
	{
		Vertex = 0,
		Fragment
	};

	using Type = std::bitset<2>;

protected:
	using Super = Shader;

protected:
	std::string m_id;
	Type m_type;

private:
	sf::Shader m_sfShader;
	bool m_bError = false;

public:
	Shader(std::string id);
	virtual ~Shader();

	void Compile(const std::string& code, Type type);
	void Compile(const std::string& vertCode, const std::string& fragCode);

	template <typename T>
	void SetUniform(std::string id, T value);

	const std::string& ID() const;
	Type GetType() const;

private:
	virtual void PreDraw(class APrimitive* pPrimitive);

	friend class Renderer;
	friend class APrimitive;
};

extern std::unordered_map<Shader::Type, std::string_view> g_szShaderTypes;

template <typename T>
void Shader::SetUniform(std::string id, T value)
{
	m_sfShader.setUniform(id, value);
}
} // namespace LittleEngine
