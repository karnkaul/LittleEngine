#include "stdafx.h"
#include "SFViewport.h"
#include "SFViewportData.h"
#include "Core/Utils.h"
#include "Core/Logger.h"
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
SFViewportSize SFViewport::GetMaxSize()
{
	sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
	return SFViewportSize{desktopMode.width, desktopMode.height};
}

SFViewport::SFViewport() = default;

SFViewport::~SFViewport()
{
	Destroy();
}

void SFViewport::SetData(SFViewportData data)
{
	Assert(!isOpen(), "Attempt to set Window Data when window already open");
	m_data = std::move(data);
}

void SFViewport::Create()
{
	Assert(!isOpen(), "Duplicate call to SFViewport::Create()!");
	create(sf::VideoMode(m_data.viewportSize.width, m_data.viewportSize.height), m_data.title,
		   Cast(m_data.style));
	m_viewSize = m_data.viewSize;
	m_viewBounds = Rect2::CentreSize(m_viewSize);
	Vector2 viewSize = m_viewBounds.GetSize();
	Fixed worldAspect = viewSize.x / viewSize.y;
	Fixed screenAspect(m_data.viewportSize.width, m_data.viewportSize.height);
	if (!Maths::IsNearlyEqual(worldAspect.ToF32(), screenAspect.ToF32()))
	{
		String screenSizeStr = "[" + Strings::ToString(m_data.viewportSize.width) + "x" +
							   Strings::ToString(m_data.viewportSize.height) + "]";
		LOG_W(
			"[SFViewport] World view's aspect ratio %s"
			" is significantly different from screen's aspect ratio %s"
			"\nExpect display to be stretched!",
			viewSize.ToString().c_str(), screenSizeStr.c_str());
	}

	sf::View view;
	view.setSize(Cast(viewSize));
	view.setCenter(Cast(Vector2::Zero));
	setView(view);
}

void SFViewport::Destroy()
{
	if (isOpen())
	{
		close();
	}
}

void SFViewport::OverrideData(SFViewportRecreateData data)
{
	if (data.oSstyle)
	{
		m_data.style = *data.oSstyle;
	}
	if (data.oTitle)
	{
		m_data.title = *data.oTitle;
	}
	if (data.oViewportSize)
	{
		m_data.viewportSize = std::move(*data.oViewportSize);
	}
}

Vector2 SFViewport::GetViewSize() const
{
	return m_viewSize;
}

Vector2 SFViewport::Project(Vector2 nPos, bool bPreClamp) const
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

Vector2 SFViewport::ScreenToWorld(sf::Vector2i screenPos) const
{
	Vector2 screenPoint(screenPos.x, screenPos.y);
	Vector2 screenSize(m_data.viewportSize.width, m_data.viewportSize.height);
	Vector2 aspectRatio(m_viewSize.x / screenSize.x, m_data.viewSize.y / screenSize.y);
	screenPoint -= (Fixed::OneHalf * screenSize);
	return Vector2(screenPoint.x * aspectRatio.x, -screenPoint.y * aspectRatio.y);
}
} // namespace LittleEngine
