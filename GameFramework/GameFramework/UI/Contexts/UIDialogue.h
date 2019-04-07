#pragma once
#include "LittleEngine/UI/UIContext.h"
#include "GameFramework/UI/Widgets/UIButton.h"

namespace LittleEngine
{
class UIDialogue : public UIContext
{
private:
	struct UIDialogueData
	{
		Vector2 size = {600, 300};
		Colour headerBG = {255, 203, 138, 255};
		Colour contentBG = Colour::White;
	};

private:
	UIDialogueData m_data;
	UIElement* m_pContent = nullptr;
	UIElement* m_pHeader = nullptr;
	UIElement* m_pFooter = nullptr;
	UIButton* m_pMainButton = nullptr;
	UIButton* m_pOtherButton = nullptr;

public:
	UIDialogue();
	UIDialogue(String name);

	UIDialogue* SetContent(const UIText& text, const Colour* pBackground = nullptr, const Vector2* pSize = nullptr);
	UIDialogue* SetHeader(const UIText& text, const Colour* pBackground = nullptr);

	UIButton::OnClick::Token AddMainButton(const UIText& text,
										   const UIButton::OnClick::Callback& onMainButton,
										   bool bDismissOnBack);
	UIButton::OnClick::Token AddOtherButton(const UIText& text,
											const UIButton::OnClick::Callback& OnOtherButton,
											bool bSelect = true);

protected:
	void OnInitContext() override;
};
} // namespace LittleEngine
