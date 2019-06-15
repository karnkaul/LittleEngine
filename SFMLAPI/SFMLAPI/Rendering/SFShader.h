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

	_COUNT,
};

extern Array<const char*, ToIdx(ShaderType::_COUNT)> g_szShaderTypes;

class SFShader
{
protected:
	enum Type
	{
		VERT = 1 << 0,
		FRAG = 1 << 1
	};

protected:
	using Super = SFShader;

protected:
	String m_id;

private:
	s32 m_type = 0;
	sf::Shader m_sfShader;
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
	virtual void PreDraw(class APrimitive* pPrimitive);

	friend class SFRenderer;
	friend class APrimitive;
};

template <typename T>
void SFShader::SetUniform(String id, T value)
{
	m_sfShader.setUniform(id, value);
}
} // namespace LittleEngine