#pragma once
#include "Core/CoreTypes.h"

namespace LittleEngine
{
class WSMErrorUI final
{
private:
	UPtr<class UIElement> m_uTitle;
	UPtr<class UIElement> m_uDescription;
	UPtr<class UIElement> m_uFooter;
	class LEContext* m_pContext;
	SPtr<s32> m_token;

public:
	WSMErrorUI(LEContext& context, bool bTerminateOnKey = true);
	~WSMErrorUI();

	void SetTitle(String title);
	void SetDescription(String description);
};
} // namespace LittleEngine