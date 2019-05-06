#pragma once
#include "Core/CoreTypes.h"
#include "SFMLAPI/Windowing/SFWindowData.h"

namespace LittleEngine
{
class SFWindow;

namespace GFX
{
void Init(const SFWindow& sfWindow);
void Cleanup();

Vector2 Project(Vector2 nPos, bool bPreClamp);
Vector2 GetViewSize();

const Map<u32, SFWindowSize>& GetValidWindowSizes();
SFWindowSize* TryGetWindowSize(u32 height);
} // namespace GFX
} // namespace LittleEngine
