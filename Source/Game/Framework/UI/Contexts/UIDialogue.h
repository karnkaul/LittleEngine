#pragma once
#include "Model/UI/UIContext.h"
#include "Framework/UI/Widgets/UIButton.h"

namespace LittleEngine
{
class UIDialogue : public UIContext
{
protected:
	struct UIDialogueData
	{
		Vector2 size = {600, 300};
		Colour headerBG = {255, 203, 138, 255};
		Colour contentBG = Colour::White;
	};

protected:
	UIDialogueData m_data;

private:
	UIElement* m_pContent = nullptr;
	UIElement* m_pHeader = nullptr;
	UIElement* m_pFooter = nullptr;
	UIButton* m_pMainButton = nullptr;
	UIButton* m_pOtherButton = nullptr;

public:
	UIDialogue* SetContent(UIText text, const Colour* pBackground = nullptr, const Vector2* pSize = nullptr);
	UIDialogue* SetHeader(UIText text, const Colour* pBackground = nullptr);

	UIButton::OnClick::Token AddMainButton(UIText text, UIButton::OnClick::Callback onMainButton, bool bDismissOnBack);
	UIButton::OnClick::Token AddOtherButton(UIText text, UIButton::OnClick::Callback onOtherButton, bool bSelect = true);

protected:
	void OnCreated() override;
};
} // namespace LittleEngine
