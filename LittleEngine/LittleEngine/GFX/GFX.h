#pragma once
#include "CoreTypes.h"

namespace LittleEngine
{
class SFWindow;

namespace GFX
{
void Init(const SFWindow& sfWindow);
void Cleanup();

Vector2 Project(const Vector2& nPos, bool bPreClamp);
Vector2 GetViewSize();
} // namespace GFX
} // namespace LittleEngine
