#include "stdafx.h"
#include "LittleEngine/LittleEngine.h"
#include "LEGame/GameFramework.h"
#include "BootWorld.h"

namespace LittleEngine
{
#if DEBUGGING
namespace
{
bool bLoadWorld1Now = false;
#if ENABLED(CONSOLE)
UPtr<Debug::Commands::Command> uUIContainerTester;
#endif
} // namespace
#endif

bool g_bTerminateOnReady = false;

BootWorld::BootWorld() : World("Boot")
{
}

void BootWorld::OnActivated()
{
#if DEBUGGING
	g_bTerminateOnReady = OS::Env()->HasVar("terminate-on-ready");
#endif

	m_pLogoFont = g_pRepository->Load<FontAsset>("Fonts/Sunscreen.otf");
#if ENABLED(CONSOLE)
	uUIContainerTester = MakeUnique<Debug::Commands::CMD_UIContainer>();
	Debug::Commands::AddCommand(std::move(uUIContainerTester));
#endif
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
		m_tokenHandler.AddToken(m_pLogoDrawer->AddButton(
			"Options", [&]() { Game()->UI()->PushContext<UIOptions>("Options"); }));
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
	if (bLoadWorld1Now)
	{
		Game()->WSM()->LoadState(1);
		bLoadWorld1Now = false;
	}
	if (g_bTerminateOnReady)
	{
		bLoadWorld1Now = true;
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
