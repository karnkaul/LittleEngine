#include "stdafx.h"
#include "ShaderRepository.h"
#include "LittleEngine/Repository/LERepository.h"

namespace LittleEngine
{
ShaderRepository* g_pShaders = nullptr;

ShaderRepoData ShaderRepository::s_data;
UMap<String, UPtr<SFShader>> ShaderRepository::s_shaderMap;

ShaderRepository::ShaderRepository(ShaderRepoData data)
{
	g_pShaders = this;
	s_data = std::move(data);
}

ShaderRepository::~ShaderRepository()
{
	g_pShaders = nullptr;
	s_shaderMap.clear();
}
} // namespace LittleEngine