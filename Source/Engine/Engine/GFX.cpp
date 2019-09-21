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

const Map<u32, ViewportSize>& GFX::ValidViewportSizes() const
{
	return viewportSizes;
}

const ViewportSize* GFX::MaxViewportSize(bool bBorderless) const
{
	Assert(!viewportSizes.empty(), "No viewport sizes stored!");
	ViewportSize hardMax = Viewport::MaxSize();
	const ViewportSize* pMax = nullptr;
	const ViewportSize* pNextMax = nullptr;
	bool bReturnNextMax = false;
	for (const auto& size : viewportSizes)
	{
		if (!pMax || size.first > pMax->height)
		{
			pNextMax = pMax;
			pMax = &size.second;
			bReturnNextMax = !bBorderless && (pMax->height >= hardMax.height || pMax->width >= hardMax.width);
		}
	}
	return bReturnNextMax ? pNextMax : pMax;
}

const ViewportSize* GFX::TryGetViewportSize(u32 height) const
{
	auto search = viewportSizes.find(height);
	if (search != viewportSizes.end())
	{
		return &search->second;
	}
	return nullptr;
}

const ViewportSize& GFX::GetViewportSize() const
{
	return viewportSize;
}

const Vector2& GFX::UISpace() const
{
	return uiSpace;
}

const sf::FloatRect& GFX::UIViewCrop() const
{
	return uiViewCrop;
}

const Vector2& GFX::WorldSpace() const
{
	return worldSpace;
}

const Vector2& GFX::OverlaySpace() const
{
	return overlaySpace;
}

f64 GFX::NativeAspectRatio() const
{
	return nativeAspectRatio;
}

f64 GFX::UIAspectRatio() const
{
	return uiAspectRatio;
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
	return Projection(nPos, uiSpace);
}

Vector2 GFX::WorldProjection(Vector2 nPos) const
{
	return Projection(nPos, worldSpace);
}

Vector2 GFX::OverlayProjection(Vector2 nPos) const
{
	return Projection(nPos, overlaySpace);
}

Vector2 GFX::WorldToUI(Vector2 world) const
{
	Vector2 full = Projection(NormalisedPosition(world, worldSpace), uiSpace);
	return Vector2(full.x * letterBoxInverse.x, full.y * letterBoxInverse.y);
}

Vector2 GFX::ViewportToWorld(s32 vpX, s32 vpY) const
{
	Vector2 vpPoint(vpX, vpY);
	Vector2 vpSize(ToS32(viewportSize.width), ToS32(viewportSize.height));
	Vector2 aspectRatio(worldSpace.x / vpSize.x, worldSpace.y / vpSize.y);
	vpPoint -= (Fixed::OneHalf * vpSize);
	return Vector2(vpPoint.x * aspectRatio.x, -vpPoint.y * aspectRatio.y);
}

void GFX::Recompute()
{
	ViewportSize max = Viewport::MaxSize();
	nativeAspectRatio = static_cast<f64>(max.width) / static_cast<f64>(max.height);
#ifdef DEBUGGING
	if (overrideNativeAR > 0.0)
	{
		nativeAspectRatio = overrideNativeAR;
	}
#endif
	uiAspectRatio = uiSpace.x.ToF64() / uiSpace.y.ToF64();
	overlaySpace = uiSpace;
	u32 viewportWidth = static_cast<u32>(viewportHeight.ToF64() * nativeAspectRatio);
	viewportSize = ViewportSize(viewportWidth, viewportHeight.ToU32());
	Fixed worldWidth(worldHeight.ToF64() * nativeAspectRatio);
	worldSpace = Vector2(worldWidth, worldHeight);
	uiViewCrop = sf::FloatRect(0.0f, 0.0f, 1.0f, 1.0f);
	if (uiAspectRatio < nativeAspectRatio)
	{
		f64 uiWidth = uiAspectRatio * viewportHeight.ToF64();
		f64 widthRatio = Maths::Abs((static_cast<f64>(viewportWidth) - uiWidth) / static_cast<f64>(viewportWidth));
		uiViewCrop.left = static_cast<f32>(widthRatio * 0.5);
		uiViewCrop.width = static_cast<f32>(1.0 - widthRatio);
		overlaySpace.x = Fixed(static_cast<f32>(uiSpace.y.ToF64() * nativeAspectRatio));
	}
	else if (uiAspectRatio > nativeAspectRatio)
	{
		f64 uiHeight = static_cast<f64>(viewportWidth) / uiAspectRatio;
		f64 heightRatio = Maths::Abs((viewportHeight.ToF64() - uiHeight) / viewportHeight.ToF64());
		uiViewCrop.top = static_cast<f32>(heightRatio * 0.5);
		uiViewCrop.height = static_cast<f32>(1.0 - heightRatio);
		overlaySpace.y = Fixed(static_cast<f32>(uiSpace.x.ToF64() / nativeAspectRatio));
	}
	letterBoxInverse = Vector2(Fixed(1.0f / uiViewCrop.width), Fixed(1.0f / uiViewCrop.height));
	viewportSizes.clear();
	Vec<Fixed> heights = {360, 540, 720, 900, 1080, 1440, 2160};
	for (auto h : heights)
	{
		u32 height = h.ToU32();
		u32 width = static_cast<u32>(h.ToF64() * nativeAspectRatio);
		if (width <= max.width && height <= max.height)
		{
			viewportSizes[height] = ViewportSize(width, height);
		}
	}
}
} // namespace LittleEngine
