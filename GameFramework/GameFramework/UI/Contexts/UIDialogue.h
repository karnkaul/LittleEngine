#pragma once
#include "LittleEngine/UI/UIContext.h"
#include "GameFramework/UI/Widgets/UIButton.h"

namespace LittleEngine
{
struct UIDialogueData
{
	UIText titleUIText;
	UIText contentUIText;
	Vector2 size = {600, 300};
	Colour headerBG = Colour::Cyan;
	Colour contentBG = Colour::White;
	bool bDestroyOnReturn;
};

class UIDialogue : public UIContext
{
private:
	UIDialogueData m_data;
	UIElement* m_pContent = nullptr;
	UIElement* m_pHeader = nullptr;
	UIElement* m_pFooter = nullptr;
	UIButton* m_pMainButton = nullptr;
	UIButton* m_pOtherButton = nullptr;

public:
	UIDialogue();
	UIDialogue(const String& name);
	virtual ~UIDialogue();

	OnClick::Token InitDialogue(UIDialogueData&& data, UIText mainButtonUIText, OnClick::Callback OnMainButton);
	OnClick::Token AddOtherButton(UIText otherButtonUIText, OnClick::Callback OnOtherButton, bool bSelect = true);
};
} // namespace LittleEngine
