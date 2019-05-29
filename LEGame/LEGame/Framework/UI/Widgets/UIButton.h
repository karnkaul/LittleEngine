#pragma once
#include "Core/Delegate.h"
#include "LEGame/GameModel/UI/UIText.h"
#include "LEGame/GameModel/UI/UIWidget.h"

namespace LittleEngine
{
class UIButton : public UIWidget
{
public:
	using OnClick = Core::Delegate<>;

private:
	OnClick m_OnInteracted;

public:
	void SetText(UIText uiText);
	OnClick::Token AddCallback(OnClick::Callback callback);

protected:
	void OnCreated() override;
	void OnSelected() override;
	void OnDeselected() override;
	void OnInteractStart() override;
	void OnInteractEnd(bool bInteract) override;
	void OnSetInteractable(bool bInteractable) override;
};
} // namespace LittleEngine
