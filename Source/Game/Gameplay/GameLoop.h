#pragma once
#include "Core/CoreTypes.h"

namespace LittleEngine
{
namespace GameLoop
{
s32 Run(s32 argc, char** argv);
bool ReloadGame(bool bHardReset);
}; // namespace GameLoop
} // namespace LittleEngine
