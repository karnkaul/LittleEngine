#include "stdafx.h"
#include "Core/Logger.h"
#include "Core/GData.h"
#include "Core/Utils.h"
#include "UIGameStyle.h"

namespace LittleEngine
{
namespace
{
UIWidgetStyle defaultStyle;
UMap<String, UIWidgetStyle> styleMap;

UByte H2B(const String& hex)
{
	s32 integer = -1;
	std::istringstream(hex) >> std::hex >> integer;
	if (integer >= 0)
	{
		return UByte(static_cast<u8>(integer));
	}
	return UByte(255);
}

Colour HexStrToColour(String hex, Colour defaultColour = Colour::White)
{
	Strings::ToLower(hex);
	if (hex.size() == 6 || hex.size() == 8)
	{
		String r = hex.substr(0, 2);
		String g = hex.substr(2, 2);
		String b = hex.substr(4, 2);
		String a = "ff";
		if (hex.size() == 8)
		{
			a = hex.substr(6, 2);
		}
		return Colour(H2B(r), H2B(g), H2B(b), H2B(a));
	}
	return defaultColour;
}

void SetupDefaultStyle()
{
	defaultStyle.widgetSize = {200, 60};
	defaultStyle.background = Colour::White;

	defaultStyle.uninteractable.fill = {150, 150, 150, 255};

	defaultStyle.notSelected.fill = {255, 135, 69, 255};

	defaultStyle.selected.fill = {255, 135, 69, 255};
	defaultStyle.selected.border = 5;
	defaultStyle.selected.outline = {18, 139, 165, 255};

	defaultStyle.interacting.fill = {255, 150, 255, 255};
}
} // namespace

void UIGameStyle::Load(String serialised)
{
	SetupDefaultStyle();

	Core::GData data(serialised);
	auto dataVec = data.GetVectorGData("styles");
	for (auto& styleData : dataVec)
	{
		UIWidgetStyle newStyle;
		String id = styleData.GetString("id");
		newStyle.widgetSize = styleData.GetVector2("size", defaultStyle.widgetSize);
		newStyle.background = ParseColour(styleData.GetString("background"));

		auto temp = styleData.GetGData("uninteractable");
		newStyle.uninteractable.fill = ParseColour(temp.GetString("fill", "#969696"));
		newStyle.uninteractable.border = temp.GetS32("border", 0);
		newStyle.uninteractable.outline = ParseColour(temp.GetString("outline", "transparent"));
		newStyle.uninteractable.textColour = ParseColour(temp.GetString("textColour", "black"));

		temp = styleData.GetGData("selected");
		newStyle.selected.fill = ParseColour(temp.GetString("fill", "#ff8745"));
		newStyle.selected.outline = ParseColour(temp.GetString("outline", "#128ba5"));
		newStyle.selected.border = temp.GetS32("border", 5);
		newStyle.selected.textColour = ParseColour(temp.GetString("textColour", "black"));

		temp = styleData.GetGData("notSelected");
		newStyle.notSelected.fill = ParseColour(temp.GetString("fill", "#ff8745"));
		newStyle.notSelected.outline = ParseColour(temp.GetString("outline", "transparent"));
		newStyle.notSelected.border = temp.GetS32("border", 0);
		newStyle.notSelected.textColour = ParseColour(temp.GetString("textColour", "black"));

		temp = styleData.GetGData("interacting");
		newStyle.interacting.fill = ParseColour(temp.GetString("fill", "#ff96ff"));
		newStyle.interacting.outline = ParseColour(temp.GetString("outline", "transparent"));
		newStyle.interacting.border = temp.GetS32("border", 0);
		newStyle.interacting.textColour = ParseColour(temp.GetString("textColour", "black"));

		if (id.empty() || id == "default")
		{
			defaultStyle = newStyle;
		}
		else
		{
			styleMap[id] = newStyle;
		}
	}
}

Colour UIGameStyle::ParseColour(String serialised)
{
	Strings::ToLower(serialised);
	if (serialised == "white")
	{
		return Colour::White;
	}
	if (serialised == "black")
	{
		return Colour::Black;
	}
	if (serialised == "red")
	{
		return Colour::Red;
	}
	if (serialised == "green")
	{
		return Colour::Green;
	}
	if (serialised == "blue")
	{
		return Colour::Blue;
	}
	if (serialised == "cyan")
	{
		return Colour::Cyan;
	}
	if (serialised == "magenta")
	{
		return Colour::Magenta;
	}
	if (serialised == "yellow")
	{
		return Colour::Yellow;
	}
	if (serialised == "transparent")
	{
		return Colour::Transparent;
	}
	if (!serialised.empty() && serialised[0] == '#')
	{
		return HexStrToColour(serialised.substr(1, serialised.length() - 1));
	}
	return Colour::White;
}

const UIWidgetStyle& UIGameStyle::GetStyle(const String& id)
{
	if (!id.empty())
	{
		auto iter = styleMap.find(id);
		if (iter != styleMap.end())
		{
			return iter->second;
		}
	}
	return defaultStyle;
}

void UIGameStyle::Overwrite(UIWidgetStyle& outStyle, const String& id)
{
	if (!id.empty())
	{
		if (styleMap.find(id) == styleMap.end())
		{
			LOG_W("[GameStyle] UIWidgetStyle %s not found", id.c_str());
		}
	}
	auto style = GetStyle(id);
	style.baseLayer = outStyle.baseLayer;
	outStyle = style;
}
} // namespace LittleEngine