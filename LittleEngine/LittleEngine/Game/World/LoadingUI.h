#pragma once
#include "LittleEngine/UI/UIElement.h"
#include "LittleEngine/UI/UIProgressBar.h"

namespace LittleEngine
{
class LoadingUI final
{
private:
	UPtr<UIElement> m_uBG;
	UPtr<UIElement> m_uTitle;
	UPtr<UIProgressBar> m_uProgressBar;

public:
	LoadingUI();
	~LoadingUI();

	void Tick(Time dt, const Fixed& progress);

private:
	void TickElements(Time dt);
};
} // namespace LittleEngine
