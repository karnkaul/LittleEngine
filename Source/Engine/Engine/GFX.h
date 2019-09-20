#pragma once
#include "Core/CoreTypes.h"
#include "SFMLAPI/Viewport/ViewportData.h"
#include "SFML/Graphics/Rect.hpp"

namespace LittleEngine
{
extern struct GFX* g_pGFX;

struct GFX final
{
private:
	// Derived
	Map<u32, ViewportSize> viewportSizes;
	ViewportSize viewportSize;
	sf::FloatRect uiViewCrop;
	Vector2 letterBoxInverse;
	Vector2 worldViewSize;
	f64 nativeAspectRatio;
	f64 uiAspectRatio;

public:
	// Set by game
	ViewportSize maxViewportSize;
	Vector2 uiViewSize;
	Fixed viewportHeight;
	Fixed worldViewHeight;
#ifdef DEBUGGING
	f64 overrideNativeAR = 0.0;
#endif

public:
	GFX();
	~GFX();

public:
	const Map<u32, ViewportSize>& ValidViewportSizes() const;
	const ViewportSize* MaxViewportSize(bool bBorderless) const;
	const ViewportSize* TryGetViewportSize(u32 height) const;

	const ViewportSize& GetViewportSize() const;
	const Vector2& UIViewSize() const;
	const sf::FloatRect& UIViewCrop() const;
	const Vector2& WorldViewSize() const;
	f64 NativeAspectRatio() const;
	f64 UIAspectRatio() const;

	Vector2 NormalisedPosition(Vector2 position, Vector2 size) const;
	Vector2 Projection(Vector2 nPos, Vector2 size) const;
	Vector2 UIProjection(Vector2 nPos) const;
	Vector2 WorldProjection(Vector2 nPos) const;
	Vector2 WorldToUI(Vector2 world) const;

	void Recompute();
};
} // namespace LittleEngine
