#pragma once
#include "Core/CoreTypes.h"
#include "UIStyle.h"

namespace LittleEngine
{
namespace UIGameStyle
{
Colour ParseColour(std::string serialised);
void Load(std::string serialised);
const UIWidgetStyle& GetStyle(const std::string& id);
void Overwrite(UIWidgetStyle& outStyle, const std::string& id);
} // namespace UIGameStyle
} // namespace LittleEngine
