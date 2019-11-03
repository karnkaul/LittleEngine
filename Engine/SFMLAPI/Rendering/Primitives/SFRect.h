#pragma once
#include "SFML/Graphics/RectangleShape.hpp"
#include "SFDrawable.h"

namespace LittleEngine
{
class SFRect : public ASFDrawable
{
protected:
	struct SFRectState
	{
		TRange<Vector2> tSize = Vector2::Zero;
	};

protected:
	SFRectState m_rectGameState;
	SFRectState m_rectRenderState;
	sf::RectangleShape m_sfRect;

public:
	SFRect(LayerID layer);
	~SFRect() override;

	// Warning: does not account for orientation!
	Rect2 GameBounds() const override;

protected:
	void SwapState() override;
	void ReconcileGameState() override;
	void OnUpdateRenderState(Fixed alpha) override;
	void OnDraw(Viewport& viewport, sf::RenderStates& sfStates) override;
	Vector2 SFSize() const override;
	sf::FloatRect SFBounds() const override;

public:
	SFRect* SetSize(Vector2 size, bool bImmediate = true);

	Vector2 Size() const;
};
} // namespace LittleEngine
