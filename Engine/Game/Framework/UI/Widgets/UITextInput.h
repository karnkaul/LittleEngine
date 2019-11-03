#pragma once
#include "Game/Model/UI/UIWidget.h"
#include "Engine/Input/LEInput.h"

namespace LittleEngine
{
class UITextInput : public UIWidget
{
public:
	using OnEditComplete = LE::Delegate<const std::string&>;

private:
	struct UITextInputData
	{
		UIText textStyle;
		Colour text = Colour::Black;
	};

private:
	UITextInputData m_data;
	std::unique_ptr<class KeyboardInput> m_uKeyboard;
	std::string m_prevText;
	Token m_inputToken;
	Token m_pointerToken;
	OnEditComplete m_onEditComplete;
	Time m_cursorFreq = Time::Milliseconds(200);
	Time m_elapsed;
	UIElement* m_pCursor = nullptr;
	UIElement* m_pText = nullptr;
	bool m_bWriting = false;
	bool m_bShowCursor = false;

public:
	UITextInput();
	~UITextInput() override;

	UITextInput* SetTextColour(Colour text);
	UITextInput* SetTextStyle(UIText uiText);
	Token SetOnEditComplete(OnEditComplete::Callback callback);

public:
	void Tick(Time dt) override;

protected:
	void OnCreated() override;
	void OnSelected() override;
	void OnDeselected() override;
	void OnInteractStart() override;
	void OnInteractEnd(bool bInteract) override;
	void OnSetInteractable(bool bInteractable) override;

private:
	bool OnInput(const LEInput::Frame& frame);
};
} // namespace LittleEngine
