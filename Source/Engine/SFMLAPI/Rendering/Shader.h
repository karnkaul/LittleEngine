#pragma once
#include <bitset>
#include "SFML/Graphics.hpp"
#include "Core/CoreTypes.h"

namespace LittleEngine
{
class Shader
{
public:
	enum Flag
	{
		VERT = 0,
		FRAG
	};

	using Type = std::bitset<2>;

protected:
	using Super = Shader;

protected:
	String m_id;
	Type m_type;

private:
	sf::Shader m_sfShader;
	bool m_bError = false;

public:
	Shader(String id);
	virtual ~Shader();

	void Compile(const String& code, Type type);
	void Compile(const String& vertCode, const String& fragCode);

	template <typename T>
	void SetUniform(String id, T value);

	const String& ID() const;
	Type GetType() const;

private:
	virtual void PreDraw(class APrimitive* pPrimitive);

	friend class Renderer;
	friend class APrimitive;
};

extern UMap<Shader::Type, const char*> g_szShaderTypes;

template <typename T>
void Shader::SetUniform(String id, T value)
{
	m_sfShader.setUniform(id, value);
}
} // namespace LittleEngine
