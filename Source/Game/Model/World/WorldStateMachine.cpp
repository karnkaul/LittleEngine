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
#include "Model/GameManager.h"
#include "Model/UI/UIGameStyle.h"

namespace LittleEngine
{
namespace
{
enum class State
{
	Idle,
	Loading,
	Running,
	Quitting
};

enum class Transition
{
	None,
	UnloadLoad,
	UnloadRun,
	LoadRun,
	Run
};

static bool bCreated = false;

u32 titleSize = 75;
u32 subtitleSize = 60;
String manifestPath;
Time loadTime;
Task onLoaded;
UPtr<class ILoadingHUD> uLoadHUD;
UPtr<class GameManager> uGame;
SFText* pLoadingTitle = nullptr;
SFText* pLoadingSubtitle = nullptr;
Quad* pSpinner = nullptr;
World* pActiveWorld = nullptr;
World* pNextWorld = nullptr;
class ManifestLoader* pLoader = nullptr;
State state;
Transition transition;
} // namespace

#ifdef DEBUGGING
bool WorldStateMachine::s_bRunning = false;
#endif

Vec<UPtr<World>> WorldStateMachine::s_createdWorlds;

WorldStateMachine::WorldStateMachine(LEContext& context) : m_pContext(&context)
{
	if (bCreated)
	{
		throw FatalEngineException();
	}

	for (auto& uWorld : s_createdWorlds)
	{
		uWorld->Init(*this);
	}
	LOG_I("[WSM] Processed %u created Worlds", s_createdWorlds.size());
	uGame = MakeUnique<GameManager>(*this);
	uLoadHUD = MakeUnique<LoadingHUD>();
	pLoadingTitle = &uLoadHUD->Title();
	pLoadingTitle->SetSize(titleSize);
	pLoadingSubtitle = uLoadHUD->Subtitle();
	if (pLoadingSubtitle)
	{
		pLoadingSubtitle->SetSize(subtitleSize);
	}
	pSpinner = uLoadHUD->Spinner();
	bCreated = true;
}

WorldStateMachine::~WorldStateMachine()
{
	UnloadActiveWorld();
	s_createdWorlds.clear();
	uGame = nullptr;
	uLoadHUD = nullptr;
	LOG_D("[WSM] destroyed");
}

void WorldStateMachine::Start(String coreManifestID, String gameStyleID, Task onManifestLoaded)
{
	manifestPath = std::move(coreManifestID);
	state = State::Loading;
	onLoaded = [&, onManifestLoaded]() {
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
			uLoadHUD->SetEnabled(true);
			m_inputToken = uGame->Input()->Register([&](const LEInput::Frame&) {
				Quit();
				return true;
			});
			pLoadingTitle->SetText("ERROR!");
			if (pLoadingSubtitle)
			{
				pLoadingSubtitle->SetText("No worlds created!");
			}
			if (pSpinner)
			{
				pSpinner->SetEnabled(false);
			}
		}
	};

	if (!manifestPath.empty() && !s_createdWorlds.empty())
	{
		pLoader = g_pRepository->LoadManifest(manifestPath, [&, gameStyleID]() {
			if (!gameStyleID.empty())
			{
				auto pText = g_pRepository->Load<TextAsset>(gameStyleID);
				if (pText)
				{
					UIGameStyle::Load(pText->Text());
				}
			}
			transition = Transition::Run;
			pLoader = nullptr;
		});
		loadTime = Time::Now();
		String titleText = LOC("LOC_LOADING");
#if ENABLED(DEBUG_LOGGING)
		titleText = LOC("LOC_LOADING_ASSETS");
#endif
		pLoadingTitle->SetText(std::move(titleText))->SetPrimaryColour(Colour::White);
		uLoadHUD->SetEnabled(true);
	}
	else
	{
		transition = Transition::Run;
	}
}

void WorldStateMachine::Tick(Time dt, bool& bYieldIntegration)
{
#ifdef DEBUGGING
	s_bRunning = state == State::Running;
#endif
	bYieldIntegration = false;
	switch (transition)
	{
	case Transition::None:
	{
		switch (state)
		{
		// Persistent states
		case State::Loading:
		{
			if (uLoadHUD)
			{
				Fixed progress = pLoader ? pLoader->Progress() : Fixed::One;
				uLoadHUD->Tick(dt, progress);
			}
			break;
		}

		case State::Running:
		{
			uLoadHUD->Tick(dt, Fixed::One);
			if (pActiveWorld && pActiveWorld->m_state == World::State::Active)
			{
				if (!(uGame->IsPaused() && !pActiveWorld->m_bTickWhenPaused))
				{
					pActiveWorld->Tick(dt);
				}
			}
			uGame->Tick(dt);
			break;
		}

		case State::Quitting:
		{
			m_pContext->Terminate();
			pNextWorld = pActiveWorld = nullptr;
			transition = Transition::None;
			state = State::Idle;
			bYieldIntegration = true;
			break;
		}

		default:
			break;
		}
		break;
	}

	default:
		ChangeState();
		bYieldIntegration = true;
	}
}

bool WorldStateMachine::LoadWorld(WorldID id)
{
	if (id >= 0 && static_cast<size_t>(id) < s_createdWorlds.size())
	{
		if (state != State::Running && state != State::Idle)
		{
			LOG_E("[WSM] Cannot Load another World while WSM is busy!");
			return false;
		}
		pNextWorld = s_createdWorlds.at(static_cast<size_t>(id)).get();
		manifestPath = pNextWorld->m_name + ".amf";
		uLoadHUD->Reset();
		pLoadingTitle->SetText(LOC("LOC_LOADING"));
		if (pLoadingSubtitle)
		{
			String key = "LOC_WORLD_" + pNextWorld->m_name;
			pLoadingSubtitle->SetText(LOC(key));
		}
		uLoadHUD->SetEnabled(true);
		if (g_pRepository->IsPresent(manifestPath))
		{
			transition = Transition::UnloadLoad;
			pNextWorld->m_state = World::State::Loading;
		}
		else
		{
			transition = Transition::UnloadRun;
			manifestPath.clear();
		}
		LOG_D("[WSM] Load Enqueued: %s", pNextWorld->LogNameStr());
		return true;
	}
	LOG_E("[WSM] World ID [%d] does not exist!", id);
	return false;
}

void WorldStateMachine::SetLoadingHUD(UPtr<ILoadingHUD> uLoadingHUD)
{
	uLoadHUD = std::move(uLoadingHUD);
}

World* WorldStateMachine::ActiveWorld() const
{
	return pActiveWorld;
}

WorldID WorldStateMachine::ActiveWorldID() const
{
	return pActiveWorld ? pActiveWorld->m_id : -1;
}

Vec<WorldID> WorldStateMachine::AllWorldIDs() const
{
	Vec<WorldID> ret;
	for (const auto& uWorld : s_createdWorlds)
	{
		ret.push_back(uWorld->m_id);
	}
	return ret;
}

void WorldStateMachine::ChangeState()
{
	switch (transition)
	{
	case Transition::UnloadLoad:
	{
		UnloadActiveWorld();
		transition = Transition::LoadRun;
		break;
	}

	case Transition::UnloadRun:
	{
		UnloadActiveWorld();
		transition = Transition::Run;
		break;
	}

	case Transition::LoadRun:
	{
		loadTime = Time::Now();
		pLoader = g_pRepository->LoadManifest(manifestPath, [&]() { transition = Transition::Run; });
		uLoadHUD->Tick(Time::Zero, Fixed::Zero);
		state = State::Loading;
		transition = Transition::None;
		break;
	}

	case Transition::Run:
	{
		if (!manifestPath.empty())
		{
			loadTime = Time::Now() - loadTime;
#if ENABLED(DEBUG_LOGGING)
			auto size = Core::FriendlySize(g_pRepository->LoadedBytes());
			LOG_D("[WSM] %s load completed in %.2fs. Repository size: %.2f%s", manifestPath.c_str(), loadTime.AsSeconds(), size.first,
				  size.second);
#endif
		}
		uLoadHUD->SetEnabled(false);
		if (pNextWorld)
		{
			pActiveWorld = pNextWorld;
			pActiveWorld->Activate();
			pNextWorld = nullptr;
		}
		state = State::Running;
		transition = Transition::None;
		if (onLoaded)
		{
			onLoaded();
			onLoaded = nullptr;
		}
		break;
	}

	default:
		break;
	}
}

void WorldStateMachine::UnloadActiveWorld()
{
	if (pActiveWorld)
	{
		pActiveWorld->Deactivate();
		String manifestID = pActiveWorld->m_manifestID;
		if (g_pRepository->IsPresent(manifestID))
		{
			m_onSubmitToken = m_pContext->RegisterOnSubmitted([manifestID]() { g_pRepository->UnloadManifest(manifestID); });
		}
	}
	uGame->Reset();
}

void WorldStateMachine::Quit()
{
	UnloadActiveWorld();
	state = State::Quitting;
}
} // namespace LittleEngine
