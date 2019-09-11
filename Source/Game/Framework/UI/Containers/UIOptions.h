#pragma once
#include "Framework/UI/Widgets/UIButton.h"
#include "UIContainer.h"

namespace LittleEngine
{
class UIOptions final : public UIContainer
{
private:
	UIButton::OnClick::Token m_worldSelectToken;
	Vec<UIButton::OnClick::Token> m_tokens;

public:
	UIOptions();
	~UIOptions() override;

protected:
	void OnCreated() override;
	void OnActivated() override;
};
} // namespace LittleEngine
