#include "Core/Logger.h"
#include "Core/Utils.h"
#include "GFX.h"
#include "SFMLAPI/Viewport/Viewport.h"

namespace LittleEngine
{
GFX* g_pGFX = nullptr;

GFX::GFX()
{
	g_pGFX = this;
}

GFX::~GFX()
{
	g_pGFX = nullptr;
}

void GFX::Init()
{
	m_maxViewportSize = Viewport::MaxSize();
	m_nativeAspectRatio = static_cast<f64>(m_maxViewportSize.width) / static_cast<f64>(m_maxViewportSize.height);
#ifdef DEBUGGING
	if (m_overrideNativeAR > 0.0)
	{
		m_nativeAspectRatio = m_overrideNativeAR;
	}
#endif
	m_uiAspectRatio = m_uiSpace.x.ToF64() / m_uiSpace.y.ToF64();
	m_overlaySpace = m_uiSpace;
	u32 viewportWidth = static_cast<u32>(m_viewportHeight.ToF64() * m_nativeAspectRatio);
	m_viewportSize = ViewportSize(viewportWidth, m_viewportHeight.ToU32());
	m_uiViewCrop = sf::FloatRect(0.0f, 0.0f, 1.0f, 1.0f);
	if (m_uiAspectRatio < m_nativeAspectRatio)
	{
		f64 uiWidth = m_uiAspectRatio * m_viewportHeight.ToF64();
		f64 widthRatio = Maths::Abs((static_cast<f64>(viewportWidth) - uiWidth) / static_cast<f64>(viewportWidth));
		m_uiViewCrop.left = static_cast<f32>(widthRatio * 0.5);
		m_uiViewCrop.width = static_cast<f32>(1.0 - widthRatio);
		m_overlaySpace.x = Fixed(static_cast<f32>(m_uiSpace.y.ToF64() * m_nativeAspectRatio));
	}
	else if (m_uiAspectRatio > m_nativeAspectRatio)
	{
		f64 uiHeight = static_cast<f64>(viewportWidth) / m_uiAspectRatio;
		f64 heightRatio = Maths::Abs((m_viewportHeight.ToF64() - uiHeight) / m_viewportHeight.ToF64());
		m_uiViewCrop.top = static_cast<f32>(heightRatio * 0.5);
		m_uiViewCrop.height = static_cast<f32>(1.0 - heightRatio);
		m_overlaySpace.y = Fixed(static_cast<f32>(m_uiSpace.x.ToF64() / m_nativeAspectRatio));
	}
	m_letterBoxInverse = Vector2(Fixed(1.0f / m_uiViewCrop.width), Fixed(1.0f / m_uiViewCrop.height));
	m_viewportSizes.clear();
	m_viewportSizes[m_maxViewportSize.height] = m_maxViewportSize;
	Vec<Fixed> heights = {360, 540, 720, 900, 1080, 1440, 2160};
	for (auto h : heights)
	{
		u32 height = h.ToU32();
		u32 width = static_cast<u32>(h.ToF64() * m_nativeAspectRatio);
		if (width <= m_maxViewportSize.width && height <= m_maxViewportSize.height)
		{
			m_viewportSizes[height] = ViewportSize(width, height);
		}
	}
	Fixed worldWidth(m_gameState.worldHeight.max.ToF64() * m_nativeAspectRatio);
	m_worldSpace = Vector2(worldWidth, m_gameState.worldHeight.max);
}

const Map<u32, ViewportSize>& GFX::ValidViewportSizes() const
{
	return m_viewportSizes;
}

const ViewportSize* GFX::MaxViewportSize(bool bBorderless) const
{
	Assert(!m_viewportSizes.empty(), "No viewport sizes stored!");
	ViewportSize hardMax = Viewport::MaxSize();
	u32 padWidth = Maths::Scale(hardMax.width, 0.9f);
	u32 padHeight = Maths::Scale(hardMax.height, 0.9f);
	const ViewportSize* pMax = nullptr;
	const ViewportSize* pNextMax = nullptr;
	bool bReturnNextMax = false;
	for (const auto& size : m_viewportSizes)
	{
		if (!pMax || size.first > pMax->height)
		{
			pNextMax = pMax;
			pMax = &size.second;
			bReturnNextMax = !bBorderless && (pMax->height > padHeight || pMax->width > padWidth);
		}
	}
	return bReturnNextMax ? pNextMax : pMax;
}

const ViewportSize* GFX::TryGetViewportSize(u32 height) const
{
	auto search = m_viewportSizes.find(height);
	if (search != m_viewportSizes.end())
	{
		return &search->second;
	}
	return nullptr;
}

const ViewportSize& GFX::GetViewportSize() const
{
	return m_viewportSize;
}

const Vector2& GFX::UISpace() const
{
	return m_uiSpace;
}

const sf::FloatRect& GFX::UIViewCrop() const
{
	return m_uiViewCrop;
}

Fixed GFX::WorldHeight() const
{
	return m_gameState.worldHeight.max;
}

const Vector2& GFX::WorldSpace() const
{
	return m_worldSpace;
}

const Vector2& GFX::OverlaySpace() const
{
	return m_overlaySpace;
}

f64 GFX::NativeAspectRatio() const
{
	return m_nativeAspectRatio;
}

f64 GFX::UIAspectRatio() const
{
	return m_uiAspectRatio;
}

Vector2 GFX::NormalisedPosition(Vector2 position, Vector2 size) const
{
	Vector2 halfSize = size * Fixed::OneHalf;
	position.x /= halfSize.x;
	position.y /= halfSize.y;
	return position;
}

Vector2 GFX::Projection(Vector2 nPos, Vector2 size) const
{
	return Vector2(nPos.x * size.x * Fixed::OneHalf, nPos.y * size.y * Fixed::OneHalf);
}

Vector2 GFX::UIProjection(Vector2 nPos) const
{
	return Projection(nPos, m_uiSpace);
}

Vector2 GFX::WorldProjection(Vector2 nPos) const
{
	return Projection(nPos, m_worldSpace);
}

Vector2 GFX::OverlayProjection(Vector2 nPos) const
{
	return Projection(nPos, m_overlaySpace);
}

Vector2 GFX::WorldToUI(Vector2 world) const
{
	Vector2 full = Projection(NormalisedPosition(world, m_worldSpace), m_uiSpace);
	return Vector2(full.x * m_letterBoxInverse.x, full.y * m_letterBoxInverse.y);
}

Vector2 GFX::WorldToOverlay(Vector2 world) const 
{
	return Projection(NormalisedPosition(world, m_worldSpace), m_overlaySpace);
}

Vector2 GFX::ViewportToWorld(s32 vpX, s32 vpY) const
{
	Vector2 vpPoint(vpX, vpY);
	Vector2 vpSize(ToS32(m_viewportSize.width), ToS32(m_viewportSize.height));
	Vector2 aspectRatio(m_worldSpace.x / vpSize.x, m_worldSpace.y / vpSize.y);
	vpPoint -= (Fixed::OneHalf * vpSize);
	return Vector2(vpPoint.x * aspectRatio.x, -vpPoint.y * aspectRatio.y);
}

void GFX::SetWorldHeight(Fixed height, bool bImmediate)
{
	if (bImmediate)
	{
		m_gameState.worldHeight.Reset(height);
	}
	else
	{
		m_gameState.worldHeight.Update(height);
	}
}

void GFX::SetWorldPosition(Vector2 position, bool bImmediate)
{
	if (bImmediate)
	{
		m_gameState.position.Reset({-position.x, position.y});
	}
	else
	{
		m_gameState.position.Update({-position.x, position.y});
	}
}

void GFX::Reconcile()
{
	m_gameState.worldHeight.min = m_gameState.worldHeight.max;
	m_gameState.position.min = m_gameState.position.max;
}

void GFX::SwapState() 
{
	m_renderState = m_gameState;
}

Vector2 GFX::LerpedWorldSpace(Fixed alpha) 
{
	Fixed worldWidth(m_renderState.worldHeight.Lerp(alpha).ToF64() * m_nativeAspectRatio);
	m_worldSpace = Vector2(worldWidth, m_gameState.worldHeight.max);
	return m_worldSpace;
}

Vector2 GFX::LerpedWorldPosition(Fixed alpha) 
{ 
	return m_renderState.position.Lerp(alpha);
}
} // namespace LittleEngine
