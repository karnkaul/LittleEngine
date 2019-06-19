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

public:
	Rect2 GetBounds(bool bWorld = false) const override;

public:
	void SwapState() override;

protected:
	void OnUpdateRenderState(Fixed alpha) override;
	void OnDraw(SFViewport& viewport, sf::RenderStates& sfStates) override;

public:
	SFRect* SetSize(Vector2 size);
};
} // namespace LittleEngine