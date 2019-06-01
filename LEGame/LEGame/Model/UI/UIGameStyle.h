#pragma once
#include "Core/CoreTypes.h"
#include "UIStyle.h"

namespace LittleEngine
{
namespace UIGameStyle
{
Colour ParseColour(String serialised);
void Load(String serialised);
const UIWidgetStyle& GetStyle(const String& id);
void Overwrite(UIWidgetStyle& outStyle, const String& id);
}
}
