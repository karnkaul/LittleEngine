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
	UPtr<UIElement> m_uEllipsis;
	UPtr<UIProgressBar> m_uProgressBar;
	String m_ellipsis;
	Time m_elapsed;

public:
	LoadingUI();
	~LoadingUI();

	void Tick(Time dt, const Fixed& progress);
	void Reset();

private:
	void TickElements(Time dt);
};
} // namespace LittleEngine
