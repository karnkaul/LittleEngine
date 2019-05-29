#pragma once
#include "Core/CoreTypes.h"
#include "Core/SimpleTime.h"

namespace LittleEngine
{
class WSMLoadingUI final
{
private:
	UPtr<class UIElement> m_uBG;
	UPtr<UIElement> m_uTitle;
	UPtr<UIElement> m_uEllipsis;
	UPtr<class UIElement> m_uProgressBar;
	class LEContext* m_pContext;
	String m_ellipsis;
	Time m_elapsed;

public:
	WSMLoadingUI(LEContext& context);
	~WSMLoadingUI();

	void Tick(Time dt, Fixed progress);
	void Reset();

private:
	void TickElements(Time dt);
};
} // namespace LittleEngine
