#include "stdafx.h"
#include "Core/Logger.h"
#include "SFShader.h"
#include "SFPrimitive.h"
#include "SFMLAPI/Viewport/SFViewport.h"

namespace LittleEngine
{
const char* g_szShaderTypes[static_cast<size_t>(ShaderType::_COUNT)] = {"Invalid", "Vertex", "Fragment", "VertFrag"};

SFShader::SFShader(String id) : m_id(std::move(id))
{
}

void SFShader::Compile(const String& code, ShaderType type)
{
	Assert(type == ShaderType::Vertex || type == ShaderType::Fragment,
		   "Can only compile single ShaderType via this overload");
	m_type = type;
	m_bError = true;
	if (sf::Shader::isAvailable())
	{
		if (!code.empty())
		{
			sf::Shader::Type sfType = type == ShaderType::Fragment ? sf::Shader::Fragment : sf::Shader::Vertex;
			if (m_shader.loadFromMemory(code, sfType))
			{
				m_bError = false;
			}
		}
	}
	if (m_bError)
	{
		m_type = ShaderType::Invalid;
		LOG_E("[%s] Error loading %s Shader!", m_id.c_str(), g_szShaderTypes[static_cast<size_t>(m_type)]);
	}
	else
	{
		m_type = type;
	}
}

void SFShader::Compile(const String& vertCode, const String& fragCode)
{
	m_bError = true;
	if (sf::Shader::isAvailable())
	{
		if (m_shader.loadFromMemory(vertCode, fragCode))
		{
			m_bError = false;
		}
	}
	if (m_bError)
	{
		LOG_E("[%s] Error loading %s Shader!", m_id.c_str(), g_szShaderTypes[static_cast<size_t>(m_type)]);
		m_type = ShaderType::Invalid;
	}
	else
	{
		m_type = ShaderType::VertFrag;
	}
}

SFShader::~SFShader()
{
	if (!m_bError)
	{
		LOG_I("-- [%s] %s Shader destroyed", m_id.c_str(), g_szShaderTypes[static_cast<size_t>(m_type)]);
	}
}

const String& SFShader::GetID() const
{
	return m_id;
}

ShaderType SFShader::GetType() const
{
	return m_type;
}

void SFShader::Draw(SFPrimitive& primitive, SFViewport& viewport)
{
	sf::RenderStates states;
	states.shader = &m_shader;
	viewport.draw(primitive.m_circle, states);
	viewport.draw(primitive.m_rectangle, states);
	viewport.draw(primitive.m_sprite, states);
	viewport.draw(primitive.m_text, states);
}
} // namespace LittleEngine