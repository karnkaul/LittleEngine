#include "Core/Logger.h"
#include "Shader.h"
#include "Primitives/Primitive.h"
#include "SFMLAPI/Viewport/Viewport.h"

namespace LittleEngine
{
UMap<Shader::Type, VString> g_szShaderTypes = {
	{Shader::Type(), "Invalid"},
	{Shader::Type().set(ToIdx(Shader::Flag::Vertex)), "Vertex"},
	{Shader::Type().set(ToIdx(Shader::Flag::Fragment)), "Fragment"},
	{Shader::Type().set(ToIdx(Shader::Flag::Vertex)).set(ToIdx(Shader::Flag::Fragment)), "VertFrag"}};

Shader::Shader(String id) : m_id(std::move(id)) {}

Shader::~Shader()
{
	if (!m_bError)
	{
		LOG_I("-- [%s] %s Shader destroyed", m_id.c_str(), g_szShaderTypes[m_type].data());
	}
}

void Shader::Compile(const String& code, Type type)
{
	Assert(type.count() == 1, "Can only compile single ShaderType via this overload");
	m_bError = true;
	if (sf::Shader::isAvailable())
	{
		if (!code.empty())
		{
			m_type = type;
			sf::Shader::Type sfType = m_type[ToIdx(Flag::Vertex)] ? sf::Shader::Vertex : sf::Shader::Fragment;
			if (m_sfShader.loadFromMemory(code, sfType))
			{
				m_bError = false;
			}
		}
	}
	if (m_bError)
	{
		m_type = 0;
		LOG_E("[%s] Error loading %s Shader!", m_id.c_str(), g_szShaderTypes[m_type].data());
	}
}

void Shader::Compile(const String& vertCode, const String& fragCode)
{
	m_bError = true;
	if (sf::Shader::isAvailable())
	{
		if (!vertCode.empty())
		{
			if (m_sfShader.loadFromMemory(vertCode, sf::Shader::Vertex))
			{
				m_bError = false;
				m_type.set(ToIdx(Flag::Vertex));
			}
		}

		if (!fragCode.empty())
		{
			if (m_sfShader.loadFromMemory(fragCode, sf::Shader::Fragment))
			{
				m_bError = false;
				m_type.set(ToIdx(Flag::Fragment));
			}
		}
	}
	if (m_bError)
	{
		LOG_E("[%s] Error loading %s Shader!", m_id.c_str(), g_szShaderTypes[m_type].data());
		m_type = 0;
	}
}

const String& Shader::ID() const
{
	return m_id;
}

Shader::Type Shader::GetType() const
{
	return m_type;
}

void Shader::PreDraw(APrimitive*)
{
	// Set uniforms here
}
} // namespace LittleEngine
