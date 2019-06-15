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
	Rect2 GetBounds() const override;

public:
	void SwapState() override;
	void OnUpdateRenderState(Fixed alpha) override;

protected:
	void OnDraw(SFViewport& viewport, sf::RenderStates& sfStates) override;

public:
	SFCircle* SetDiameter(Fixed diameter);
};
} // namespace LittleEngine