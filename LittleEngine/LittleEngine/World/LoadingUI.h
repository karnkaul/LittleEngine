#pragma once
#include "LittleEngine/UI/UIElement.h"

namespace LittleEngine
{
class LoadingUI final
{
private:
	String m_ellipsesText = "";
	UPtr<UIElement> m_uBG;
	UPtr<UIElement> m_uTitle;
	UPtr<UIElement> m_uEllipses;
	Time m_ellipsesElapsed;

public:
	LoadingUI();
	~LoadingUI();

	void Tick(Time dt);

private:
	void TickElements(Time dt);
};
} // namespace LittleEngine
