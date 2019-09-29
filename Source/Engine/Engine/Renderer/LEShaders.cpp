#include "LEShaders.h"

namespace LittleEngine
{
LEShaders* g_pShaders = nullptr;

ShadersData LEShaders::s_data;
UMap<String, UPtr<Shader>> LEShaders::s_shaderMap;

LEShaders::LEShaders(ShadersData data)
{
	g_pShaders = this;
	s_data = std::move(data);
}

LEShaders::~LEShaders()
{
	g_pShaders = nullptr;
	UnloadAll();
}

void LEShaders::UnloadAll()
{
	s_shaderMap.clear();
}
} // namespace LittleEngine
