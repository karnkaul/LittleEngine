#include "stdafx.h"
#include "UISelection.h"
#include "GameFramework/UI/Contexts/UIButtonDrawer.h"
#include "LittleEngine/Services/Services.h"
#include "LittleEngine/Game/GameManager.h"

namespace LittleEngine
{
UISelection::Option::Option(const String& value, size_t idx) : value(value), idx(static_cast<u32>(idx))
{
}

UISelection::UISelection() : UIButton("Untitled")
{
	SetName("", "UISelection");
}

UISelection::UISelection(const String& name) : UIButton(name)
{
	SetName("", "UISelection");
}

UISelection::~UISelection()
{
	LOG_D("%s destroyed", LogNameStr());
}

UISelection::OnChanged::Token UISelection::SetOnChanged(const OnChanged::Callback& callback)
{
	return m_onChanged.Register(callback);
}

UISelection* UISelection::SetValue(const String& text)
{
	m_value = text;
	SetText(m_value);
	return this;
}

UISelection* UISelection::AddOption(const String& option)
{
	m_options.push_back(option);
	return this;
}

UISelection* UISelection::AddOptions(const Vec<String>& options)
{
	m_options = options;
	return this;
}

UISelection* UISelection::AddOptions(Vec<String>&& options)
{
	m_options = std::move(options);
	return this;
}

UISelection* UISelection::SetPanelSize(const Vector2& size)
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

void UISelection::OnInitWidget()
{
	m_style = UIWidgetStyle::GetDefault1(&m_style);
	
	UIButton::OnInitWidget();

	m_buttonToken = AddCallback(std::bind(&UISelection::OnSpawnDrawer, this));
}

void UISelection::OnSpawnDrawer()
{
	LayerID drawerLayer = static_cast<LayerID>(m_style.baseLayer + 2);
	m_pDrawer = Services::Game()->UI()->PushContext<UIButtonDrawer>(drawerLayer);
	UIStyle panelStyle;
	panelStyle.size = m_data.panelSize;
	panelStyle.fill = m_data.panelColour;
	m_pDrawer->SetPanel(panelStyle);
	m_pDrawer->m_bAutoDestroyOnCancel = true;

	for (size_t idx = 0; idx < m_options.size(); ++idx)
	{
		String option = m_options[idx];
		m_drawerTokens.push_back(m_pDrawer->AddButton(option, [&, idx, option]() 
		{ 
			m_pDrawer->Destruct();
			m_value = option;
			SetText(m_value);
			Option selected(m_value, idx);
			m_onChanged(selected);
		}));
	}

	m_pDrawer->SetActive(true);
}
} // namespace LittleEngine