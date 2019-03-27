#pragma once
#include "CoreTypes.h"
#include "SimpleTime.h"

namespace LittleEngine
{
class LoadingUI final
{
private:
	UPtr<class UIElement> m_uBG;
	UPtr<UIElement> m_uTitle;
	UPtr<UIElement> m_uEllipsis;
	UPtr<class UIProgressBar> m_uProgressBar;
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
