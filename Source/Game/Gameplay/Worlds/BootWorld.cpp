#include "Engine/Engine.h"
#include "Framework/Framework.h"
#include "BootWorld.h"

namespace LittleEngine
{
#if defined(DEBUGGING)
namespace
{
bool bLoadWorld1Now = false;
bool bMusicPlaying = false;
#if ENABLED(CONSOLE)
UPtr<Debug::Commands::Command> uUIContainerTester;
#endif

LEInput::Token token;
bool MusicTest(const LEInput::Frame& frame)
{
	static const Array<String, 2> ids = {"TestMusic.ogg", "TestMusic_0.ogg"};
	static size_t idx = 0;
	if (frame.IsReleased(KeyCode::X))
	{
		if (!bMusicPlaying)
		{
			g_pAudio->PlayMusic(ids[idx]);
			bMusicPlaying = true;
			return true;
		}
		idx = (idx == ids.size() - 1) ? 0 : idx + 1;
		g_pAudio->SwitchTrack(ids[idx]);
	}
	return false;
}
} // namespace
#endif

bool g_bTerminateOnReady = false;

BootWorld::BootWorld() : World("Boot") {}

void BootWorld::OnActivated()
{
#if defined(DEBUGGING)
	g_bTerminateOnReady = OS::Env()->HasVar("terminate-on-ready");
#endif

	m_pLogoFont = g_pRepository->Load<FontAsset>("Fonts/Sunscreen.otf");
#if ENABLED(CONSOLE)
	uUIContainerTester = MakeUnique<Debug::Commands::CMD_UIContainer>();
	Debug::Commands::AddCommand(std::move(uUIContainerTester));
#endif
	if ((m_pLogoDrawer = g_pGameManager->UI()->PushContext<UIButtonDrawer>("MainMenu")))
	{
		m_pLogoHeader = m_pLogoDrawer->AddElement<UIElement>("Logo Header");
		m_pLogoHeader->SetText(UIText("Little Engine", 80, Colour::White));
		if (m_pLogoFont)
		{
			m_pLogoHeader->SetFont(*m_pLogoFont);
		}
		m_pLogoHeader->m_transform.UnsetParent();
		m_pLogoHeader->m_transform.nPosition = {0, Fixed(0.8f)};
		m_tokenHandler.AddToken(m_pLogoDrawer->AddButton(LOC("LOC_START"), [&]() { LoadWorld(m_id + 1); }));
		m_tokenHandler.AddToken(
			m_pLogoDrawer->AddButton(LOC("LOC_OPTIONS"), [&]() { g_pGameManager->UI()->PushContext<UIOptions>("Options"); }));
		m_tokenHandler.AddToken(m_pLogoDrawer->AddButton(LOC("LOC_QUIT"), [&]() { Quit(); }));

		m_pLogoDrawer->ResetSelection();
	}

	if (!g_pAudio->IsMusicPlaying())
	{
		// g_pAudio->PlayMusic("TestMusic.ogg", Fixed::OneHalf);
	}
#if defined(DEBUGGING)
	token = Input()->Register(&MusicTest);
#endif
}

void BootWorld::Tick(Time /*dt*/)
{
	// Fade logo
	if (m_pLogoDrawer && m_pLogoHeader)
	{
		Fixed seconds = Fixed(WorldTime().AsSeconds());
		Fixed speed = 2;
		Fixed alpha = (seconds * speed).Sin().Abs() * 255;
		Colour colour = m_pLogoHeader->Text()->GetColour();
		colour.a = UByte(static_cast<u8>(alpha.ToU32()));
		m_pLogoHeader->Text()->SetColour(colour);
	}
#if defined(DEBUGGING)
	if (bLoadWorld1Now)
	{
		WorldID self = g_pGameManager->ActiveWorldID();
		LoadWorld(self + 1);
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
#if defined(DEBUGGING)
	token = nullptr;
#endif
}
} // namespace LittleEngine
