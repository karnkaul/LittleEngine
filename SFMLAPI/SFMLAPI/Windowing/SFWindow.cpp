#include "stdafx.h"
#include "SFWindow.h"
#include "SFWindowData.h"
#include "Utils.h"
#include "Logger.h"
#include "SFMLAPI/System/SFTypes.h"

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
SFWindowSize SFWindow::GetMaxWindowSize()
{
	sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
	return SFWindowSize(desktopMode.width, desktopMode.height);
}

SFWindow::SFWindow(const SFWindowData& data)
	: RenderWindow(sf::VideoMode(data.windowSize.width, data.windowSize.height), data.title, data.sfStyle)
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

SFWindow::~SFWindow() = default;

Vector2 SFWindow::GetViewSize() const
{
	return m_viewBounds.GetSize();
}

Vector2 SFWindow::Project(Vector2 nPos, bool bPreClamp) const
{
	Vector2 p = nPos;
	if (bPreClamp)
	{
		p.x = Maths::Clamp_11(p.x);
		p.y = Maths::Clamp_11(p.y);
	}
	Vector2 s = m_viewBounds.topRight;
	return Vector2(p.x * s.x, p.y * s.y);
}

void SFWindow::SetSize(const SFWindowSize& size)
{
	sf::Vector2u fullSize = getSize();
	sf::Vector2i halfSize(fullSize.x / 2, fullSize.y / 2);
	sf::Vector2i position = getPosition() + halfSize;
	setSize({size.width, size.height});
	halfSize = sf::Vector2i(size.width / 2, size.height / 2);
	position -= halfSize;
	setPosition(position);
}
} // namespace LittleEngine
