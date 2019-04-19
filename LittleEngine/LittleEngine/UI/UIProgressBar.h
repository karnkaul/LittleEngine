#pragma once
#include "UIElement.h"

namespace LittleEngine
{
class UIProgressBar : public UIElement
{
protected:
	Vector2 m_size;

public:
	UIProgressBar(LayerID layer, bool bSilent = false);
	
	void InitProgressBar(Vector2 size, Colour colour, Fixed initProgress = Fixed::Zero);
	void SetProgress(Fixed progress);

protected:
	void OnCreated() override;
};
} // namespace LittleEngine
