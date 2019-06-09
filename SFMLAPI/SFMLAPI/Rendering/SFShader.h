#pragma once
#include "SFML/Graphics.hpp"
#include "Core/CoreTypes.h"

namespace LittleEngine
{
enum class ShaderType
{
	Invalid,
	Vertex,
	Fragment,
	VertFrag,
	_COUNT
};

extern const char* g_szShaderTypes[static_cast<size_t>(ShaderType::_COUNT)];

class SFShader
{
protected:
	using Super = SFShader;

protected:
	String m_id;
private:
	ShaderType m_type = ShaderType::Invalid;
	sf::Shader m_shader;
	bool m_bError = false;

public:
	SFShader(String id);
	virtual ~SFShader();

	void Compile(const String& code, ShaderType type);
	void Compile(const String& vertCode, const String& fragCode);
	
	template <typename T>
	void SetUniform(String id, T value);
	
	const String& GetID() const;
	ShaderType GetType() const;

private:
	virtual void Draw(class SFPrimitive& primitive, class SFViewport& viewport);

	friend class SFRenderer;
};

template <typename T>
void SFShader::SetUniform(String id, T value)
{
	m_shader.setUniform(id, value);
}
}