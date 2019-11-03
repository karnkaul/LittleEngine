#include "LEShaders.h"

namespace LittleEngine
{
namespace LEShaders
{
ShadersData s_data;
std::unordered_map<std::string, std::unique_ptr<Shader>> s_shaderMap;
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
