#include "stdafx.h"
#include "UIStyle.h"

namespace LittleEngine
{
UIWidgetStyle UIWidgetStyle::GetDefault()
{
	UIWidgetStyle ret;
	ret.widgetSize = {200, 60};

	ret.uninteractable.fill = {150, 150, 150, 255};

	ret.notSelected.fill = {255, 135, 69, 255};

	ret.selected.fill = {255, 135, 69, 255};
	ret.selected.border = 5;
	ret.selected.outline = {18, 139, 165, 255};

	ret.interacting.fill = {255, 150, 255, 255};

	return ret;
}
} // namespace LittleEngine