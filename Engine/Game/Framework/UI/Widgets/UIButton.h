#pragma once
#include "Game/Model/UI/UIText.h"
#include "Game/Model/UI/UIWidget.h"

namespace LittleEngine
{
class UIButton : public UIWidget
{
public:
	using OnClick = LE::Delegate<>;

protected:
	bool m_bAutoResize = true;

private:
	OnClick m_OnInteracted;

public:
	void SetText(UIText uiText);
	Token AddCallback(OnClick::Callback callback);

public:
	void Tick(Time dt = Time::Zero) override;

protected:
	void OnCreated() override;
	void OnSelected() override;
	void OnDeselected() override;
	void OnInteractStart() override;
	void OnInteractEnd(bool bInteract) override;
	void OnSetInteractable(bool bInteractable) override;

private:
	void Resize();
};
} // namespace LittleEngine
