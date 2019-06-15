#pragma once
#include "SFML/Graphics/RenderStates.hpp"
#include "Core/CoreTypes.h"
#include "Core/TRange.h"
#include "SFMLAPI/Rendering/Colour.h"
#include "SFMLAPI/Rendering/SFLayerID.h"
#include "SFMLAPI/Viewport/SFViewport.h"

namespace LittleEngine
{
class APrimitive
{
protected:
	template <typename T>
	using TRange = Core::TRange<T>;

	struct TState
	{
		TRange<Vector2> tPosition = Vector2::Zero;
		TRange<Vector2> tScale = Vector2::One;
		TRange<Fixed> tOrientation = Fixed::Zero;
		TRange<Colour> tColour = Colour(255, 255, 255, 255);
		
		bool bEnabled = false;

		void Reconcile();
	};

	struct State
	{
		Vector2 position;
		Vector2 scale;
		Fixed orientation;
		Colour colour;
	};

#if DEBUGGING
		public : bool m_bDEBUG = false;
#endif
protected:
	TState m_gameState;
	TState m_renderState;

public:
	const LayerID m_layer = LAYER_DEFAULT;

protected:
	class SFShader* m_pShader = nullptr;
	bool m_bDestroyed = false;
	bool m_bWasDisabled = false;
	bool m_bStatic = false;
	bool m_bMakeStatic = false;
	bool m_bRendered = false;

public:
	APrimitive(LayerID layer);
	virtual ~APrimitive();

protected:
	virtual void OnUpdateRenderState(Fixed alpha) = 0;
	virtual void OnSwapState() = 0;
	virtual void OnDraw(SFViewport& viewport, sf::RenderStates& sfStates) = 0;

public:
	virtual Rect2 GetBounds() const = 0;
	virtual void ReconcileGameState();

public:
	template <typename T>
	T* CastTo();

public:
	void SwapState();
	void UpdateRenderState(Fixed alpha);
	void Destroy();

private:
	void Draw(SFViewport& viewport, Fixed alpha);

public:
	// Interpolated states
	APrimitive* SetPosition(Vector2 worldPosition, bool bImmediate = false);
	APrimitive* SetOrientation(Fixed worldOrientation, bool bImmediate = false);
	APrimitive* SetScale(Vector2 worldScale, bool bImmediate = false);
	APrimitive* SetPrimaryColour(Colour colour, bool bImmediate = false);
	
	// Regular states
	APrimitive* SetEnabled(bool bEnabled);
	APrimitive* SetShader(SFShader* pShader);
	APrimitive* SetStatic(bool bStatic);

	bool IsDestroyed() const;
	bool IsEnabled() const;
	bool IsStatic() const;

	Vector2 GetPosition() const;
	Fixed GetOrientation() const;
	Vector2 GetScale() const;
	Colour GetPrimaryColour() const;

protected:
	State GetState(Fixed alpha) const;

private:
	friend class SFRenderer;
};

template <typename T>
T* APrimitive::CastTo()
{
	static_assert(IsDerived<APrimitive, T>(), "T must derive from APrimitive");
	return dynamic_cast<T*>(this);
}
} // namespace LittleEngine
