#include "stdafx.h"
#include "Core/Logger.h"
#include "SFShader.h"
#include "SFPrimitive.h"
#include "SFMLAPI/Viewport/SFViewport.h"

namespace LittleEngine
{
const char* g_szShaderTypes[ToIdx(ShaderType::_COUNT)] = {"Invalid", "Vertex",
																		"Fragment", "VertFrag"};

SFShader::SFShader(String id) : m_id(std::move(id))
{
}

void SFShader::Compile(const String& code, ShaderType type)
{
	Assert(type == ShaderType::Vertex || type == ShaderType::Fragment,
		   "Can only compile single ShaderType via this overload");
	m_bError = true;
	if (sf::Shader::isAvailable())
	{
		if (!code.empty())
		{
			sf::Shader::Type sfType = sf::Shader::Vertex;
			switch (type)
			{
			default:
			case ShaderType::Vertex:
				m_type = VERT;
				sfType = sf::Shader::Vertex;
				break;

			case ShaderType::Fragment:
				m_type |= FRAG;
				sfType = sf::Shader::Fragment;
				break;
			}
			if (m_shader.loadFromMemory(code, sfType))
			{
				m_bError = false;
			}
		}
	}
	if (m_bError)
	{
		m_type = 0;
		LOG_E("[%s] Error loading %s Shader!", m_id.c_str(), g_szShaderTypes[ToIdx(m_type)]);
	}
}

void SFShader::Compile(const String& vertCode, const String& fragCode)
{
	m_bError = true;
	if (sf::Shader::isAvailable())
	{
		if (fragCode.empty() && !vertCode.empty())
		{
			if (m_shader.loadFromMemory(vertCode, sf::Shader::Vertex))
			{
				m_bError = false;
				m_type = VERT;
			}
		}

		else if (vertCode.empty() && !fragCode.empty())
		{
			if (m_shader.loadFromMemory(fragCode, sf::Shader::Fragment))
			{
				m_bError = false;
				m_type = FRAG;
			}
		}

		else if (!vertCode.empty() && !fragCode.empty())
		{
			if (m_shader.loadFromMemory(vertCode, fragCode))
			{
				m_bError = false;
				m_type = VERT | FRAG;
			}
		}
	}
	if (m_bError)
	{
		LOG_E("[%s] Error loading %s Shader!", m_id.c_str(), g_szShaderTypes[ToIdx(m_type)]);
		m_type = 0;
	}
}

SFShader::~SFShader()
{
	if (!m_bError)
	{
		LOG_I("-- [%s] %s Shader destroyed", m_id.c_str(), g_szShaderTypes[ToIdx(m_type)]);
	}
}

const String& SFShader::GetID() const
{
	return m_id;
}

ShaderType SFShader::GetType() const
{
	if (m_type == VERT)
	{
		return ShaderType::Vertex;
	}
	if (m_type == FRAG)
	{
		return ShaderType::Fragment;
	}
	if (m_type & VERT && m_type & FRAG)
	{
		return ShaderType::VertFrag;
	}
	return ShaderType::Invalid;
}

void SFShader::Draw(SFPrimitive& primitive, SFViewport& viewport)
{
	sf::RenderStates states;
	states.shader = &m_shader;
	if (primitive.m_flags & SFPrimitive::SHAPE)
	{
		viewport.draw(primitive.m_circle, states);
		viewport.draw(primitive.m_rectangle, states);
	}
	else if (primitive.m_flags & SFPrimitive::SPRITE)
	{
		viewport.draw(primitive.m_sprite, states);
	}
	else if (primitive.m_flags & SFPrimitive::TEXT)
	{
		viewport.draw(primitive.m_text, states);
	}
}
} // namespace LittleEngine