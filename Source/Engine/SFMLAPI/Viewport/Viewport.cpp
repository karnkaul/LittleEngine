#include "Viewport.h"
#include "ViewportData.h"
#include "Core/Utils.h"
#include "Core/Logger.h"
#include "SFMLAPI/System/SFTypes.h"

namespace LittleEngine
{
ViewportSize Viewport::MaxSize()
{
	sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
	return ViewportSize{desktopMode.width, desktopMode.height};
}

Viewport::Viewport() = default;

Viewport::~Viewport()
{
	Destroy();
}

void Viewport::SetData(ViewportData data)
{
	Assert(!isOpen(), "Attempt to set Window Data when window already open");
	m_data = std::move(data);
}

void Viewport::Create(u32 framerateLimit)
{
	Assert(!isOpen(), "Duplicate call to Viewport::Create()!");
	auto max = MaxSize();
	sf::Vector2u vp(m_data.viewportSize.width, m_data.viewportSize.height);
	if (vp.x > max.width || vp.y > max.height)
	{
		LOG_E("[Viewport] Viewport size [%ux%u] too large to fit on screen [%ux%u]!", vp.x, vp.y, max.width, max.height);
		vp = sf::Vector2u(max.width, max.height);
	}

	create(sf::VideoMode(vp.x, vp.y), m_data.title, Cast(m_data.style));
	setVerticalSyncEnabled(true);
	setFramerateLimit(framerateLimit);
	m_viewSize = m_data.viewSize;
	m_viewBounds = Rect2::SizeCentre(m_viewSize);
	Vector2 viewSize = m_viewBounds.Size();
	Fixed worldAspect = viewSize.x / viewSize.y;
	Fixed vpAspect(static_cast<s32>(m_data.viewportSize.width), static_cast<s32>(m_data.viewportSize.height));
	if (!Maths::IsNearlyEqual(worldAspect.ToF32(), vpAspect.ToF32(), 0.1f))
	{
		LOG_W("[Viewport] World view's aspect ratio %s"
			  " is significantly different from screen's aspect ratio %s"
			  "\nExpect display to be stretched!",
			  viewSize.ToString().c_str(), m_data.viewportSize.ToString().c_str());
	}

	sf::View view;
	view.setSize(Cast(viewSize));
	view.setCenter(Cast(Vector2::Zero));
	setView(view);

	sf::Vector2i c(static_cast<s32>(max.width - vp.x) / 2, static_cast<s32>(max.height - vp.y) / 2);
	setPosition(c);

	LOG_D("[Viewport] created. Pixel size: %s. View size: %s", m_data.viewportSize.ToString().c_str(), viewSize.ToString().c_str());
}

void Viewport::Destroy()
{
	if (isOpen())
	{
		close();
	}
}

void Viewport::OverrideData(ViewportRecreateData data)
{
	if (data.oStyle)
	{
		m_data.style = *data.oStyle;
	}
	if (data.oTitle)
	{
		m_data.title = *data.oTitle;
	}
	if (data.oViewportSize)
	{
		m_data.viewportSize = *data.oViewportSize;
	}
}

Vector2 Viewport::ViewSize() const
{
	return m_viewSize;
}

Vector2 Viewport::Project(Vector2 nPos, bool bPreClamp) const
{
	Vector2 p = nPos;
	if (bPreClamp)
	{
		p.x = Maths::Clamp_11(p.x);
		p.y = Maths::Clamp_11(p.y);
	}
	Vector2 s = m_viewBounds.TopRight();
	return Vector2(p.x * s.x, p.y * s.y);
}

Vector2 Viewport::ViewportToWorld(s32 vpX, s32 vpY) const
{
	Vector2 vpPoint(vpX, vpY);
	Vector2 vpSize(static_cast<s32>(m_data.viewportSize.width), static_cast<s32>(m_data.viewportSize.height));
	Vector2 aspectRatio(m_viewSize.x / vpSize.x, m_data.viewSize.y / vpSize.y);
	vpPoint -= (Fixed::OneHalf * vpSize);
	return Vector2(vpPoint.x * aspectRatio.x, -vpPoint.y * aspectRatio.y);
}
} // namespace LittleEngine
