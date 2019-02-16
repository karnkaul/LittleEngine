#include "stdafx.h"
#include "SFWindow.h"
#include "SFMLAPI/System/SFTypes.h"
#include "Utils.h"
#include "Logger.h"

#pragma comment(lib, "freetype.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "gdi32.lib")
#if defined(_DEBUG)
#pragma comment(lib, "sfml-graphics-s-d.lib")
#pragma comment(lib, "sfml-window-s-d.lib")
#pragma comment(lib, "sfml-system-s-d.lib")
#else
#pragma comment(lib, "sfml-graphics-s.lib")
#pragma comment(lib, "sfml-window-s.lib")
#pragma comment(lib, "sfml-system-s.lib")
#endif

namespace LittleEngine
{
SFWindow::SFWindow(const SFWindowData& data)
	: RenderWindow(sf::VideoMode(data.windowSize.width, data.windowSize.height), data.title)
{
	m_viewBounds = Rect2::CentreSize(data.viewSize);
	Vector2 viewSize = m_viewBounds.GetSize();
	Fixed worldAspect = viewSize.x / viewSize.y;
	Fixed screenAspect(data.windowSize.width, data.windowSize.height);
	if (!Maths::IsNearlyEqual(worldAspect.ToF32(), screenAspect.ToF32()))
	{
		String screenSizeStr = "[" + Strings::ToString(data.windowSize.width) + "x" +
							   Strings::ToString(data.windowSize.height) + "]";
		LOG_W(
			"[SFWindow] World view's aspect ratio %s"
			" is significantly different from screen's aspect ratio %s"
			"\nExpect display to be stretched!",
			viewSize.ToString().c_str(), screenSizeStr.c_str());
	}

	sf::View view;
	view.setSize(Cast(viewSize));
	view.setCenter(Cast(Vector2::Zero));
	setView(view);
}

Vector2 SFWindow::GetViewSize() const
{
	return m_viewBounds.GetSize();
}

Vector2 SFWindow::Project(const Vector2& nPos, bool bPreClamp) const
{
	Vector2 p = nPos;
	if (bPreClamp)
	{
		p.x = Maths::Clamp_11(p.x);
		p.y = Maths::Clamp_11(p.y);
	}
	const Vector2& s = m_viewBounds.topRight;
	return Vector2(p.x * s.x, p.y * s.y);
}
} // namespace LittleEngine
