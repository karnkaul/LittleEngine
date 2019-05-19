#pragma once
#include "Core/Delegate.h"
#include "LittleEngine/UI/UIWidget.h"
#include "LittleEngine/UI/UIText.h"

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

public:
	void SetInteractable(bool bInteractable) override;

protected:
	void OnCreated() override;
	void OnSelected() override;
	void OnDeselected() override;
	void OnInteractStart() override;
	void OnInteractEnd(bool bInteract) override;
};
} // namespace LittleEngine
