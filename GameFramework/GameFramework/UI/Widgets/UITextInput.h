#pragma once
#include "Delegate.hpp"
#include "LittleEngine/UI/UIWidget.h"
#include "LittleEngine/Input/EngineInput.h"
#include "LittleEngine/UI/UIText.h"

namespace LittleEngine
{
class UITextInput : public UIWidget
{
public:
	using OnEditComplete = Core::Delegate<const String&>;

private:
	struct UITextInputData
	{
		UIText textStyle;
		Colour text = Colour::Black;
	};

private:
	UITextInputData m_data;
	UPtr<class KeyboardInput> m_uKeyboard;
	String m_prevText;
	EngineInput::Token m_token;
	OnEditComplete m_onEditComplete;
	Time m_cursorFreq = Time::Milliseconds(200);
	Time m_elapsed;
	UIElement* m_pRoot;
	UIElement* m_pText;
	bool m_bWriting = false;
	bool m_bShowCursor;

public:
	UITextInput();
	UITextInput(const String& name);
	~UITextInput();

	UITextInput* SetTextColour(Colour text);
	UITextInput* SetTextStyle(const UIText& uiText);
	OnEditComplete::Token SetOnEditComplete(const OnEditComplete::Callback& callback);

public:
	virtual void SetInteractable(bool bInteractable) override;

protected:
	virtual void OnInitWidget() override;
	virtual void OnSelected() override;
	virtual void OnDeselected() override;
	virtual void OnInteractStart() override;
	virtual void OnInteractEnd(bool bInteract) override;
	virtual void Tick(Time dt) override;

private:
	bool OnInput(const EngineInput::Frame& frame);
};
}