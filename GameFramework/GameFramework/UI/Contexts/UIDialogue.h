#pragma once
#include "LittleEngine/UI/UIContext.h"
#include "GameFramework/UI/Widgets/UIButton.h"

namespace LittleEngine
{
struct UIDialogueData
{
	UIButtonData buttonData;
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

	Delegate::Token InitDialogue(UIDialogueData&& data, UIText mainButtonUIText, Delegate::Callback OnMainButton);
	Delegate::Token AddOtherButton(UIText otherButtonUIText, Delegate::Callback OnOtherButton, bool bSelect = true);
};
} // namespace LittleEngine
