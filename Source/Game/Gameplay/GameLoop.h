#pragma once
#include "Core/CoreTypes.h"

namespace LittleEngine
{
namespace GameLoop
{
enum class ReloadType
{
	World = 0,
	Game,
	App,
	_COUNT
};

s32 Run(s32 argc, char** argv);
bool Reload(ReloadType type);
}; // namespace GameLoop
} // namespace LittleEngine
