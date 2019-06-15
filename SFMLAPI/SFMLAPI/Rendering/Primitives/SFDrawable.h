#pragma once
#include "SFML/System/Vector2.hpp"
#include "SFML/Graphics/Color.hpp"
#include "Primitive.h"

namespace LittleEngine
{
class ASFDrawable : public APrimitive
{
protected:
	struct LerpedState
	{
		sf::Vector2f origin;
		sf::Vector2f scale;
		sf::Vector2f position;
		sf::Color primary;
		sf::Color secondary;
		f32 orientation;
		f32 outline;
	};

protected:
	LerpedState m_lerpedState;
	Fixed m_outline;

public:
	ASFDrawable(LayerID layer);
	~ASFDrawable() override;

public:
	ASFDrawable* SetOutline(Fixed outline);

protected:
	void OnUpdateRenderState(Fixed alpha) override;
};
}
