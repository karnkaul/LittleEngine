#include "LEShaders.h"

namespace LittleEngine
{
namespace LEShaders
{
ShadersData s_data;
UMap<String, UPtr<Shader>> s_shaderMap;
} // namespace LEShaders

void LEShaders::Init(ShadersData data)
{
	s_data = std::move(data);
	s_shaderMap.reserve(128);
}

void LEShaders::Clear()
{
	s_shaderMap.clear();
}
} // namespace LittleEngine
