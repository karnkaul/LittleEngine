#pragma once
#include "Core/CoreTypes.h"
#include "Core/TRange.h"
#include "SFMLAPI/Viewport/ViewportData.h"
#include "SFML/Graphics/Rect.hpp"

namespace LittleEngine
{
extern class GFX* g_pGFX;

class GFX final
{
private:
	template <typename T>
	using TRange = Core::TRange<T>;

	struct TState
	{
		TRange<Fixed> worldHeight = Fixed::Zero;
		TRange<Vector2> position = Vector2::Zero;
	};

private:
	TState m_renderState;
	TState m_gameState;
	// Derived
	Map<u32, ViewportSize> m_viewportSizes;
	ViewportSize m_maxViewportSize;
	ViewportSize m_viewportSize;
	sf::FloatRect m_uiViewCrop;
	Vector2 m_letterBoxInverse;
	Vector2 m_worldSpace;
	Vector2 m_overlaySpace;
	f64 m_nativeAspectRatio;
	f64 m_uiAspectRatio;

public:
	// Set by game
	Vector2 m_uiSpace;
	Fixed m_viewportHeight;
#ifdef DEBUGGING
	f64 m_overrideNativeAR = 0.0;
#endif

public:
	GFX();
	~GFX();

public:
	void Init();
	
	const Map<u32, ViewportSize>& ValidViewportSizes() const;
	const ViewportSize* MaxViewportSize(bool bBorderless) const;
	const ViewportSize* TryGetViewportSize(u32 height) const;

	const ViewportSize& GetViewportSize() const;
	const Vector2& UISpace() const;
	const sf::FloatRect& UIViewCrop() const;
	Fixed WorldHeight() const;
	const Vector2& WorldSpace() const;
	const Vector2& OverlaySpace() const;
	f64 NativeAspectRatio() const;
	f64 UIAspectRatio() const;

	Vector2 NormalisedPosition(Vector2 position, Vector2 size) const;
	Vector2 Projection(Vector2 nPos, Vector2 size) const;
	Vector2 UIProjection(Vector2 nPos) const;
	Vector2 WorldProjection(Vector2 nPos) const;
	Vector2 OverlayProjection(Vector2 nPos) const;
	Vector2 WorldToUI(Vector2 world) const;
	Vector2 WorldToOverlay(Vector2 world) const;
	Vector2 ViewportToWorld(s32 vpX, s32 vpY) const;

	void SetWorldHeight(Fixed height, bool bImmediate);
	void SetWorldPosition(Vector2 position, bool bImmediate);

	void Reconcile();
	void SwapState();
	Vector2 LerpedWorldSpace(Fixed alpha);
	Vector2 LerpedWorldPosition(Fixed alpha);
};
} // namespace LittleEngine
