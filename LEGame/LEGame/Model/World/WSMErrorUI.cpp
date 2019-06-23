#include "stdafx.h"
#include "WSMErrorUI.h"
#include "LittleEngine/Context/LEContext.h"
#include "LittleEngine/Input/LEInput.h"
#include "LEGame/Model/UI/UIText.h"
#include "LEGame/Model/UI/UIElement.h"

namespace LittleEngine
{
WSMErrorUI::WSMErrorUI(LEContext& context, bool bTerminateOnKey) : m_pContext(&context)
{
	Time dt;
	m_uTitle = MakeUnique<UIElement>();
	m_uTitle->OnCreate(context, "WSMErrorTitle");
	m_uTitle->m_transform.nPosition = {0, Fixed::OneHalf};
	m_uTitle->Tick(dt);

	m_uDescription = MakeUnique<UIElement>();
	m_uDescription->OnCreate(context, "WSMErrorDesc");
	m_uDescription->Tick(dt);

	if (bTerminateOnKey)
	{
		m_uFooter = MakeUnique<UIElement>();
		m_uFooter->OnCreate(context, "WSMErrorFooter");
		m_uFooter->m_transform.nPosition = {0, -Fixed::OneThird * 2};
		m_uFooter->SetText(UIText("Press any key to quit...", 15, Colour(200, 200, 200)));
		m_uFooter->Tick(dt);
		m_token = m_pContext->Input()->Register([&](const LEInput::Frame& /*frame*/) -> bool {
			m_pContext->Terminate();
			return true;
		});
	}
}

WSMErrorUI::~WSMErrorUI()
{
	m_uTitle = nullptr;
	m_uDescription = nullptr;
}

void WSMErrorUI::SetTitle(String title)
{
	m_uTitle->SetText(UIText(std::move(title), 100, Colour(255, 100, 100)));
}

void WSMErrorUI::SetDescription(String description)
{
	m_uDescription->SetText(UIText(std::move(description), 25, Colour::White));
}
} // namespace LittleEngine