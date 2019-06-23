#include "stdafx.h"
#include "SFMLAPI/Viewport/Viewport.h"
#include "SFMLAPI/Viewport/ViewportData.h"
#include "OS.h"

namespace OS
{
Screen* g_pScreen;

Screen* Screen::Instance()
{
	using namespace LittleEngine;
	static Screen screen;
	if (screen.height == 0 || screen.width == 0)
	{
		ViewportSize windowSize = Viewport::GetMaxSize();
		screen.height = windowSize.height;
		screen.width = windowSize.width;
	}
	return &screen;
}
} // namespace OS