#pragma once
#include "UIElement.h"

namespace LittleEngine
{
class UIProgressBar : public UIElement
{
protected:
	Vector2 m_size;

public:
	UIProgressBar(bool bSilent = false);
	UIProgressBar(String name, bool bSilent = false);
	~UIProgressBar() override;

	void InitProgressBar(Vector2 size, Colour colour, Fixed initProgress = Fixed::Zero);
	void SetProgress(Fixed progress);
};
} // namespace LittleEngine
