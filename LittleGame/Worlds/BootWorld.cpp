#include "stdafx.h"
#include "GameFramework/GameFramework.h"
#include "BootWorld.h"
#include "UI/OptionsUI.h"

namespace LittleEngine
{

#if DEBUGGING
namespace
{
bool bTerminateOnReady = false;
bool bTerminateNow = false;
}
#endif

BootWorld::BootWorld() : World("Boot")
{
#if DEBUGGING
	bTerminateOnReady = OS::Env()->HasVar("terminate-on-ready");
#endif
}

void BootWorld::OnActivated()
{
	m_pLogoFont = Repository()->Load<FontAsset>("Fonts/Sunscreen.otf");
	if ((m_pLogoDrawer = Game()->UI()->PushContext<UIButtonDrawer>("MainMenu")))
	{
		m_pLogoHeader = m_pLogoDrawer->AddElement<UIElement>("Logo Header");
		m_pLogoHeader->SetText(UIText("Little Engine", 80, Colour::White));
		if (m_pLogoFont)
		{
			m_pLogoHeader->SetFont(*m_pLogoFont);
		}
		m_pLogoHeader->m_transform.UnsetParent();
		m_pLogoHeader->m_transform.nPosition = {0, Fixed(0.8f)};
		m_tokenHandler.AddToken(m_pLogoDrawer->AddButton("Start", [&]() { OnLoadNextWorld(); }));
		m_tokenHandler
			.AddToken(m_pLogoDrawer->AddButton(
				"Options", []() { Services::Game()->UI()->PushContext<OptionsUI>(); }));
		m_tokenHandler.AddToken(m_pLogoDrawer->AddButton("Quit", [&]() { Quit(); }));
		m_pLogoDrawer->SetActive(true);
	}
}

void BootWorld::Tick(Time dt)
{
	World::Tick(dt);

	// Fade logo
	if (m_pLogoDrawer && m_pLogoHeader)
	{
		Fixed seconds = Fixed(GetWorldTime().AsSeconds());
		Fixed speed = 2;
		Fixed alpha = (seconds * speed).Sin().Abs() * 255;
		Colour colour = m_pLogoHeader->GetText()->GetPrimaryColour();
		colour.a = UByte(alpha.ToU32());
		m_pLogoHeader->GetText()->SetPrimaryColour(colour);
	}
#if DEBUGGING
	if (bTerminateNow)
	{
		Services::Engine()->Terminate();
		bTerminateNow = false;
	}
	if (bTerminateOnReady)
	{
		bTerminateNow = true;
		bTerminateOnReady = false;
	}
#endif
}

void BootWorld::OnDeactivating()
{
	m_pLogoDrawer = nullptr;
	m_pLogoHeader = nullptr;
}

void BootWorld::OnLoadNextWorld()
{
	LoadWorld(m_id + 1);
}
} // namespace LittleEngine
