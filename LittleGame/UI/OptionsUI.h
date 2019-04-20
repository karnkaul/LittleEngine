#pragma once
#include "GameFramework/UI/Contexts/UIDialogue.h"

namespace LittleEngine
{
class OptionsUI : public UIDialogue
{
public:
	static bool s_bShowQuitButton;

private:
	Vec<UIButton::OnClick::Token> m_tokens;

public:
	void OnCreated() override;
};
} // namespace LittleEngine