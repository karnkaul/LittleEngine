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
	struct State
	{
		template <typename T>
		using TRange = Core::TRange<T>;
		using SFPosition = TRange<Vector2>;
		using SFOrientation = TRange<Fixed>;
		using SFScale = TRange<Vector2>;
		using SFColour = TRange<Colour>;

		SFPosition sfPosition = Vector2::Zero;
		SFOrientation sfOrientation = Fixed::Zero;
		SFScale sfScale = Vector2::One;
		SFColour sfPrimaryColour = Colour(255, 255, 255, 255);
		SFColour sfSecondaryColour = Colour(255, 255, 255, 0);
		Vector2 pivot = Vector2::Zero;
		bool bEnabled = false;

		void Reconcile();
	};

#if DEBUGGING
public:
	bool m_bDEBUG = false;
#endif
protected:
	State m_gameState;
	State m_renderState;
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
	APrimitive* SetPrimaryColour(Colour sfColour, bool bImmediate = false);
	APrimitive* SetSecondaryColour(Colour sfColour, bool bImmediate = false);
	
	// Regular states
	APrimitive* SetEnabled(bool bEnabled);
	APrimitive* SetPivot(Vector2 pivot);
	APrimitive* SetShader(SFShader* pShader);
	APrimitive* SetStatic(bool bStatic);

	bool IsDestroyed() const;
	bool IsEnabled() const;
	bool IsStatic() const;

	Vector2 GetPosition() const;
	Fixed GetOrientation() const;
	Vector2 GetScale() const;
	Colour GetPrimaryColour() const;
	Colour GetSecondaryColour() const;

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
