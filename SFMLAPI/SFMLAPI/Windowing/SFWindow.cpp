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

SFWindow::SFWindow() = default;

SFWindow::~SFWindow()
{
	DestroyWindow();
}

void SFWindow::SetData(SFWindowData data)
{
	Assert(!isOpen(), "Attempt to set Window Data when window already open");
	m_data = std::move(data);
}

void SFWindow::CreateWindow()
{
	Assert(!isOpen(), "Duplicate call to SFWindow::Create()!");
	create(sf::VideoMode(m_data.windowSize.width, m_data.windowSize.height), m_data.title,
		   Cast(m_data.style));
	m_viewBounds = Rect2::CentreSize(m_data.viewSize);
	Vector2 viewSize = m_viewBounds.GetSize();
	Fixed worldAspect = viewSize.x / viewSize.y;
	Fixed screenAspect(m_data.windowSize.width, m_data.windowSize.height);
	if (!Maths::IsNearlyEqual(worldAspect.ToF32(), screenAspect.ToF32()))
	{
		String screenSizeStr = "[" + Strings::ToString(m_data.windowSize.width) + "x" +
							   Strings::ToString(m_data.windowSize.height) + "]";
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

void SFWindow::DestroyWindow()
{
	if (isOpen())
	{
		close();
	}
}

void SFWindow::OverrideData(SFWindowRecreateData data)
{
	if (data.style)
	{
		m_data.style = *data.style;
	}
	if (data.title)
	{
		m_data.title = *data.title;
	}
	if (data.windowSize)
	{
		m_data.windowSize = std::move(*data.windowSize);
	}
}

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
} // namespace LittleEngine
