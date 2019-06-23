#pragma once
#include "SFML/Graphics/RenderStates.hpp"
#include "Core/CoreTypes.h"
#include "Core/TRange.h"
#include "SFMLAPI/Rendering/Colour.h"
#include "SFMLAPI/Rendering/LayerID.h"
#include "SFMLAPI/Viewport/Viewport.h"

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
		TRange<Vector2> tOrientation = Vector2::Right;
		TRange<Colour> tColour = Colour(255, 255, 255, 255);

		bool bEnabled = false;

		void Reconcile();
	};

	struct State
	{
		Vector2 position;
		Vector2 orientation;
		Vector2 scale;
		Colour colour;
	};

#if DEBUGGING
public:
	bool m_bDEBUG = false;
#endif
protected:
	TState m_gameState;
	TState m_renderState;

public:
	const LayerID m_layer = LAYER_DEFAULT;

protected:
	class Shader* m_pShader = nullptr;
	bool m_bDestroyed = false;
	bool m_bWasDisabled = false;
	bool m_bStatic = false;
	bool m_bMakeStatic = false;
	bool m_bRendered = false;

public:
	APrimitive(LayerID layer);
	virtual ~APrimitive();

public:
	virtual Rect2 GetBounds() const = 0;

public:
	virtual void ReconcileGameState();
	virtual void SwapState();
	void UpdateRenderState(Fixed alpha);

protected:
	virtual void OnUpdateRenderState(Fixed alpha) = 0;
	virtual void OnDraw(Viewport& viewport, sf::RenderStates& sfStates) = 0;

public:
	template <typename T>
	T* CastTo();

public:
	void Destroy();

private:
	void Draw(Viewport& viewport, Fixed alpha);

public:
	// Interpolated states
	APrimitive* SetPosition(Vector2 position, bool bImmediate = false);
	APrimitive* SetOrientation(Vector2 orientation, bool bImmediate = false);
	APrimitive* SetScale(Vector2 scale, bool bImmediate = false);
	APrimitive* SetPrimaryColour(Colour colour, bool bImmediate = false);

	// Regular states
	APrimitive* SetEnabled(bool bEnabled);
	APrimitive* SetShader(Shader* pShader);
	APrimitive* SetStatic(bool bStatic);

	bool IsDestroyed() const;
	bool IsEnabled() const;
	bool IsStatic() const;

	Vector2 GetPosition() const;
	Vector2 GetOrientation() const;
	Vector2 GetScale() const;
	Colour GetPrimaryColour() const;

protected:
	inline State GetState(Fixed alpha) const
	{
		Vector2 s = m_renderState.tScale.Lerp(alpha);
		Vector2 o = m_renderState.tOrientation.Lerp(alpha);
		Vector2 p = m_renderState.tPosition.Lerp(alpha);
		Colour c = Colour::Lerp(m_renderState.tColour, alpha);
		return {p, o, s, c};
	}

	void SetDirty(bool bReconcile);

private:
	friend class Renderer;
};

template <typename T>
T* APrimitive::CastTo()
{
	static_assert(IsDerived<APrimitive, T>(), "T must derive from APrimitive");
	return dynamic_cast<T*>(this);
}
} // namespace LittleEngine
