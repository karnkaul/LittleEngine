#include "stdafx.h"
#include "LittleEngine/Game/GameManager.h"
#include "LittleEngine/Services/Services.h"
#include "LittleEngine/UI/UIGameStyle.h"
#include "LittleEngine/UI/UIManager.h"
#include "LittleEngine/UI/UIText.h"
#include "UISelection.h"
#include "GameFramework/UI/Contexts/UIButtonDrawer.h"

namespace LittleEngine
{
UISelection::OnChanged::Token UISelection::RegisterOnChanged(OnChanged::Callback callback)
{
	return m_onChanged.Register(std::move(callback));
}

UISelection* UISelection::SetValue(String text)
{
	m_value = std::move(text);
	SetText(m_value);
	return this;
}

UISelection* UISelection::AddOption(String option)
{
	m_options.emplace_back(std::move(option));
	return this;
}

UISelection* UISelection::SetOptions(Vec<String> options)
{
	m_options = std::move(options);
	return this;
}

UISelection* UISelection::SetPanelSize(Vector2 size)
{
	m_data.panelSize = size;
	return this;
}

UISelection* UISelection::SetPanelColour(Colour colour)
{
	m_data.panelColour = colour;
	return this;
}

const String& UISelection::GetCurrentValue() const
{
	return m_value;
}

Vec<String>& UISelection::GetOptions()
{
	return m_options;
}

void UISelection::OnCreated()
{
	UIGameStyle::Overwrite(m_style, "alternate0");
	UIButton::OnCreated();

	SetType("UISelection");
	m_buttonToken = AddCallback([&]() { OnSpawnDrawer(); });
}

void UISelection::OnSpawnDrawer()
{
	m_pDrawer = Services::Game()->UI()->PushContext<UIButtonDrawer>();
	UIStyle panelStyle;
	panelStyle.size = m_data.panelSize;
	panelStyle.fill = m_data.panelColour;
	m_pDrawer->SetPanel(panelStyle);
	m_pDrawer->m_bAutoDestroyOnCancel = true;

	for (size_t idx = 0; idx < m_options.size(); ++idx)
	{
		String option = m_options[idx];
		m_drawerTokens.push_back(m_pDrawer->AddButton(option, [&, idx, option]() {
			m_pDrawer->Destruct();
			m_value = option;
			SetText(m_value);
			m_onChanged(std::make_pair(idx, option));
		}));
	}

	m_pDrawer->SetActive(true);
}
} // namespace LittleEngine