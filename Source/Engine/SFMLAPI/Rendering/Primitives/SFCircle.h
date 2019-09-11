#pragma once
#include "SFML/Graphics/CircleShape.hpp"
#include "SFDrawable.h"

namespace LittleEngine
{
class SFCircle : public ASFDrawable
{
protected:
	struct SFCircleState
	{
		Fixed diameter;
	};

protected:
	SFCircleState m_circleGameState;
	SFCircleState m_circleRenderState;
	sf::CircleShape m_sfCircle;

public:
	SFCircle(LayerID layer);
	~SFCircle() override;

public:
	void SwapState() override;
	Rect2 GameBounds() const override;

protected:
	void OnUpdateRenderState(Fixed alpha) override;
	void OnDraw(Viewport& viewport, sf::RenderStates& sfStates) override;
	Vector2 SFSize() const override;
	sf::FloatRect SFBounds() const override;

public:
	SFCircle* SetDiameter(Fixed diameter);

	Fixed Diameter() const;
};
} // namespace LittleEngine
