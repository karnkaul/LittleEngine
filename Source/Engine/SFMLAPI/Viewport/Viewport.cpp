#include "Viewport.h"
#include "ViewportData.h"
#include "Core/Utils.h"
#include "Core/Logger.h"
#include "SFMLAPI/System/SFTypes.h"
#include "Engine/GFX.h"

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

	sf::Vector2i c(static_cast<s32>(max.width - vp.x) / 2, static_cast<s32>(max.height - vp.y) / 2);
	setPosition(c);

	LOG_D("[Viewport] created %s", m_data.viewportSize.ToString().c_str());
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

Vector2 Viewport::ViewportToWorld(s32 vpX, s32 vpY) const
{
	Vector2 vpPoint(vpX, vpY);
	Vector2 vpSize = Cast(getSize());
	//Vector2 viewSize = Cast(getView().getSize());
	Vector2 worldViewSize = g_pGFX->WorldViewSize();
	Vector2 aspectRatio(worldViewSize.x / vpSize.x, worldViewSize.y / vpSize.y);
	vpPoint -= (Fixed::OneHalf * vpSize);
	return Vector2(vpPoint.x * aspectRatio.x, -vpPoint.y * aspectRatio.y);
}
} // namespace LittleEngine
