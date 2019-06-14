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
		Vector2 size;
	};

protected:
	SFRectState m_rectGameState;
	SFRectState m_rectRenderState;
	sf::RectangleShape m_sfRect;

public:
	SFRect(LayerID layer);
	~SFRect() override;

protected:
	void OnUpdateRenderState(Fixed alpha) override;
	void OnDraw(SFViewport& viewport, sf::RenderStates& sfStates) override;
	void OnSwapState() override;

public:
	Rect2 GetBounds() const override;
	
public:
	SFRect* SetSize(Vector2 size);
};
}