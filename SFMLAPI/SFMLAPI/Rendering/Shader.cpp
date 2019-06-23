#include "stdafx.h"
#include "Core/Logger.h"
#include "Shader.h"
#include "Primitives/Primitive.h"
#include "SFMLAPI/Viewport/Viewport.h"

namespace LittleEngine
{
Array<const char*, ToIdx(ShaderType::_COUNT)> g_szShaderTypes = {"Invalid", "Vertex",
																		"Fragment", "VertFrag"};

Shader::Shader(String id) : m_id(std::move(id))
{
}

void Shader::Compile(const String& code, ShaderType type)
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
			if (m_sfShader.loadFromMemory(code, sfType))
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

void Shader::Compile(const String& vertCode, const String& fragCode)
{
	m_bError = true;
	if (sf::Shader::isAvailable())
	{
		if (fragCode.empty() && !vertCode.empty())
		{
			if (m_sfShader.loadFromMemory(vertCode, sf::Shader::Vertex))
			{
				m_bError = false;
				m_type = VERT;
			}
		}

		else if (vertCode.empty() && !fragCode.empty())
		{
			if (m_sfShader.loadFromMemory(fragCode, sf::Shader::Fragment))
			{
				m_bError = false;
				m_type = FRAG;
			}
		}

		else if (!vertCode.empty() && !fragCode.empty())
		{
			if (m_sfShader.loadFromMemory(vertCode, fragCode))
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

Shader::~Shader()
{
	if (!m_bError)
	{
		LOG_I("-- [%s] %s Shader destroyed", m_id.c_str(), g_szShaderTypes[ToIdx(m_type)]);
	}
}

const String& Shader::GetID() const
{
	return m_id;
}

ShaderType Shader::GetType() const
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

void Shader::PreDraw(APrimitive*)
{
	// Set uniforms here
}
} // namespace LittleEngine