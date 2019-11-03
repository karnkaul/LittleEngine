#pragma once
#include "SFML/System/Vector2.hpp"
#include "SFML/Graphics/Color.hpp"
#include "Primitive.h"

namespace LittleEngine
{
class ASFDrawable : public APrimitive
{
protected:
	struct TDrawableState
	{
		TRange<Vector2> tPivot = Vector2::Zero;
		TRange<Colour> tSecondaryColour = Colour::Transparent;
		TRange<Fixed> tOutline = Fixed::Zero;

		void Reconcile();
	};

	struct DrawableState
	{
		Vector2 origin;
		Colour primary;
		Colour secondary;
		Fixed outline;
	};

protected:
	TDrawableState m_drawableGameState;
	TDrawableState m_drawableRenderState;

public:
	ASFDrawable(LayerID layer);
	~ASFDrawable() override;

public:
	ASFDrawable* SetPivot(Vector2 pivot, bool bImmediate = false);
	ASFDrawable* SetOutline(Fixed outline, bool bImmediate = false);
	ASFDrawable* SetSecondaryColour(Colour colour, bool bImmediate = false);

	Vector2 Pivot() const;
	Fixed Outline() const;
	Colour SecondaryColour() const;

public:
	Rect2 RenderBounds() const override;

protected:
	void ReconcileGameState() override;
	void SwapState() override;

protected:
	DrawableState GetDrawableState(Fixed alpha) const;

protected:
	virtual sf::FloatRect SFBounds() const = 0;
	virtual Vector2 SFSize() const = 0;
};
} // namespace LittleEngine
