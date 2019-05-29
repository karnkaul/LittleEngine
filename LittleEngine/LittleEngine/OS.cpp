#include "stdafx.h"
#include "SFMLAPI/Viewport/SFViewport.h"
#include "SFMLAPI/Viewport/SFViewportData.h"
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
		SFViewportSize windowSize = SFViewport::GetMaxSize();
		screen.height = windowSize.height;
		screen.width = windowSize.width;
	}
	return &screen;
}
}