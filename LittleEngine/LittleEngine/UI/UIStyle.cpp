#include "stdafx.h"
#include "UIStyle.h"

namespace LittleEngine
{
/**
	255	135	69	pastel orange
	18	139	165	pastel blue
	255	150	255	pink
	154	209	212	light cyan
	204	219	220	off white
	147	129	255	purple
	2	47	64	dark teal
	239	121	138	pinkish orange
 */

UIWidgetStyle UIWidgetStyle::GetDefault0(UIWidgetStyle* pReplace)
{
	UIWidgetStyle ret;
	if (pReplace)
		ret.baseLayer = pReplace->baseLayer;
	ret.widgetSize = {200, 60};

	ret.uninteractable.fill = {150, 150, 150, 255};

	ret.notSelected.fill = {255, 135, 69, 255};

	ret.selected.fill = {255, 135, 69, 255};
	ret.selected.border = 5;
	ret.selected.outline = {18, 139, 165, 255};

	ret.interacting.fill = {255, 150, 255, 255};

	return ret;
}

LittleEngine::UIWidgetStyle UIWidgetStyle::GetDefault1(UIWidgetStyle* pReplace)
{
	UIWidgetStyle ret;
	if (pReplace)
		ret.baseLayer = pReplace->baseLayer;
	ret.widgetSize = {200, 60};

	ret.uninteractable.fill = {150, 150, 150, 255};

	ret.notSelected.fill = {239, 121, 138, 255};

	ret.selected.fill = {239, 121, 138, 255};
	ret.selected.border = 5;
	ret.selected.outline = {18, 139, 165, 255};

	ret.interacting.fill = {255, 150, 255, 255};

	return ret;
}

} // namespace LittleEngine