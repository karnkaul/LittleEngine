#include "Core/ArchiveReader.h"
#include "Core/Logger.h"
#include "SFMLAPI/System/Assets.h"
#include "SFMLAPI/Rendering/Primitives/Quad.h"
#include "SFMLAPI/Rendering/Primitives/SFText.h"
#include "LoadingHUD.h"
#include "WorldStateMachine.h"
#include "Engine/FatalEngineException.h"
#include "Engine/Context/LEContext.h"
#include "Engine/Locale/Locale.h"
#include "Engine/Repository/LERepository.h"
#include "Engine/Repository/ManifestLoader.h"
#include "Model/GameKernel.h"
#include "Model/GameManager.h"
#include "Model/UI/UIGameStyle.h"

namespace LittleEngine
{
namespace
{
static bool bCreated = false;
u32 titleSize = 75;
u32 subtitleSize = 60;
} // namespace

#ifdef DEBUGGING
bool WorldStateMachine::s_bRunning = false;
#endif

std::vector<UPtr<World>> WorldStateMachine::s_createdWorlds;

WorldStateMachine::WorldStateMachine(LEContext& context) : m_pContext(&context)
{
	Assert(&context, "Context is null!");
	if (bCreated)
	{
		throw FatalEngineException();
	}

	for (auto& uWorld : s_createdWorlds)
	{
		uWorld->Init(*this);
	}
	LOG_I("[WSM] Processed %u created Worlds", s_createdWorlds.size());
	bCreated = true;
}

WorldStateMachine::~WorldStateMachine()
{
	UnloadActiveWorld();
	s_createdWorlds.clear();
	m_uLoadHUD = nullptr;
	bCreated = false;
	LOG_D("[WSM] destroyed");
}

void WorldStateMachine::Start(std::string coreManifestID, std::string gameStyleID, Task onManifestLoaded)
{
	Assert(g_pGameManager, "GameManager is null!");
	if (!m_uLoadHUD)
	{
		m_uLoadHUD = MakeUnique<LoadingHUD>();
		m_pLoadingTitle = &m_uLoadHUD->Title();
		m_pLoadingTitle->SetSize(titleSize);
		m_pLoadingSubtitle = m_uLoadHUD->Subtitle();
		if (m_pLoadingSubtitle)
		{
			m_pLoadingSubtitle->SetSize(subtitleSize);
		}
		m_pSpinner = m_uLoadHUD->Spinner();
	}
	m_manifestPath = std::move(coreManifestID);
	m_state = State::Loading;
	m_onLoaded = [&, onManifestLoaded]() {
		if (!s_createdWorlds.empty())
		{
			if (onManifestLoaded)
			{
				onManifestLoaded();
			}
			LoadWorld(0);
		}
		else
		{
			LOG_E("[WSM] No Worlds created! Call "
				  "`WorldStateMachine::CreateWorlds<T...>()` before `GameLoop::Run()`");
			m_uLoadHUD->SetEnabled(true);
			m_inputToken = g_pGameManager->Input()->Register([&](const LEInput::Frame&) {
				Quit();
				return true;
			});
			m_pLoadingTitle->SetText("ERROR!");
			if (m_pLoadingSubtitle)
			{
				m_pLoadingSubtitle->SetText("No worlds created!");
			}
			if (m_pSpinner)
			{
				m_pSpinner->SetEnabled(false);
			}
		}
	};

	if (!m_manifestPath.empty() && !s_createdWorlds.empty())
	{
		m_sLoader = g_pRepository->LoadManifest(m_manifestPath, [&, gameStyleID]() {
			if (!gameStyleID.empty())
			{
				auto pText = g_pRepository->Load<TextAsset>(gameStyleID);
				if (pText)
				{
					UIGameStyle::Load(pText->Text());
				}
			}
			m_transition = Transition::Run;
			m_sLoader = nullptr;
		});
		m_loadTime = Time::Now();
		std::string titleText = LOC("LOC_LOADING");
#if ENABLED(DEBUG_LOGGING)
		titleText = LOC("LOC_LOADING_ASSETS");
#endif
		m_pLoadingTitle->SetText(std::move(titleText))->SetColour(Colour::White);
		m_uLoadHUD->SetEnabled(true);
	}
	else
	{
		m_transition = Transition::Run;
	}
}

WorldStateMachine::State WorldStateMachine::Tick(Time dt)
{
#ifdef DEBUGGING
	s_bRunning = m_state == State::Running;
#endif
	switch (m_transition)
	{
	case Transition::None:
	{
		switch (m_state)
		{
		// Persistent states
		case State::Loading:
		{
			if (m_uLoadHUD)
			{
				Fixed progress = m_sLoader ? m_sLoader->Progress() : Fixed::One;
				m_uLoadHUD->Tick(dt, progress);
			}
			break;
		}

		case State::Running:
		{
			m_uLoadHUD->Tick(dt, Fixed::One);
			if (m_pActiveWorld && m_pActiveWorld->m_state == World::State::Active)
			{
				if (!(g_pGameManager->IsPaused() && !m_pActiveWorld->m_bTickWhenPaused))
				{
					m_pActiveWorld->Tick(dt);
				}
			}
			break;
		}

		case State::Quitting:
		{
			m_pContext->Terminate();
			m_pNextWorld = m_pActiveWorld = nullptr;
			m_transition = Transition::None;
			m_state = State::Idle;
			break;
		}

		default:
			break;
		}
		break;
	}

	default:
		ChangeState();
	}
	return m_state;
}

bool WorldStateMachine::LoadWorld(WorldID id)
{
	if (id >= 0 && static_cast<size_t>(id) < s_createdWorlds.size())
	{
		if (m_state != State::Running && m_state != State::Idle)
		{
			LOG_E("[WSM] Cannot Load another World while WSM is busy!");
			return false;
		}
		m_pNextWorld = s_createdWorlds.at(static_cast<size_t>(id)).get();
		m_manifestPath = m_pNextWorld->m_name;
		m_manifestPath += ".amf";
		m_uLoadHUD->Reset();
		m_pLoadingTitle->SetText(LOC("LOC_LOADING"));
		if (m_pLoadingSubtitle)
		{
			std::string key = "LOC_WORLD_" + m_pNextWorld->m_name;
			m_pLoadingSubtitle->SetText(LOC(key));
		}
		m_uLoadHUD->SetEnabled(true);
		if (g_pRepository->IsPresent(m_manifestPath))
		{
			m_transition = Transition::UnloadLoad;
			m_pNextWorld->m_state = World::State::Loading;
		}
		else
		{
			m_transition = Transition::UnloadRun;
			m_manifestPath.clear();
		}
		LOG_D("[WSM] Load Enqueued: %s", m_pNextWorld->LogName().data());
		return true;
	}
	LOG_E("[WSM] World ID [%d] does not exist!", id);
	return false;
}

void WorldStateMachine::SetLoadingHUD(UPtr<ILoadingHUD> uLoadingHUD)
{
	m_uLoadHUD->SetEnabled(false);
	m_uLoadHUD = std::move(uLoadingHUD);
}

World* WorldStateMachine::ActiveWorld() const
{
	return m_pActiveWorld;
}

WorldID WorldStateMachine::ActiveWorldID() const
{
	return m_pActiveWorld ? m_pActiveWorld->m_id : -1;
}

std::vector<WorldID> WorldStateMachine::AllWorldIDs() const
{
	std::vector<WorldID> ret;
	for (const auto& uWorld : s_createdWorlds)
	{
		ret.push_back(uWorld->m_id);
	}
	return ret;
}

void WorldStateMachine::ChangeState()
{
	switch (m_transition)
	{
	case Transition::UnloadLoad:
	{
		UnloadActiveWorld();
		m_transition = Transition::LoadRun;
		break;
	}

	case Transition::UnloadRun:
	{
		UnloadActiveWorld();
		m_transition = Transition::Run;
		break;
	}

	case Transition::LoadRun:
	{
		m_loadTime = Time::Now();
		m_sLoader = g_pRepository->LoadManifest(m_manifestPath, [&]() { m_transition = Transition::Run; });
		m_uLoadHUD->Tick(Time::Zero, Fixed::Zero);
		m_state = State::Loading;
		m_transition = Transition::None;
		break;
	}

	case Transition::Run:
	{
		if (!m_manifestPath.empty())
		{
			m_loadTime = Time::Now() - m_loadTime;
#if ENABLED(DEBUG_LOGGING)
			auto size = Core::FriendlySize(g_pRepository->LoadedBytes());
			LOG_D("[WSM] %s load completed in %.2fs. Repository size: %.2f%s", m_manifestPath.c_str(), m_loadTime.AsSeconds(), size.first,
				  size.second.data());
#endif
		}
		m_uLoadHUD->SetEnabled(false);
		if (m_pNextWorld)
		{
			m_pActiveWorld = m_pNextWorld;
			m_pActiveWorld->Activate();
			m_pNextWorld = nullptr;
		}
		m_state = State::Running;
		m_transition = Transition::None;
		if (m_onLoaded)
		{
			m_onLoaded();
			m_onLoaded = nullptr;
		}
		break;
	}

	default:
		break;
	}
}

void WorldStateMachine::UnloadActiveWorld()
{
	if (m_pActiveWorld)
	{
		m_pActiveWorld->Deactivate();
		std::string manifestID = m_pActiveWorld->m_manifestID;
		if (g_pRepository->IsPresent(manifestID))
		{
			Assert(m_pContext, "Context is null!");
			m_onSubmitToken = m_pContext->RegisterOnSubmitted([manifestID]() { g_pRepository->UnloadManifest(manifestID); });
		}
	}
	g_pGameManager->OnWorldUnloaded();
}

void WorldStateMachine::Quit()
{
	UnloadActiveWorld();
	m_state = State::Quitting;
}
} // namespace LittleEngine
