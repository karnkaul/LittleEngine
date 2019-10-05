#include "Core/Asserts.h"
#include "Core/OS.h"
#include "Engine/Context/LEContext.h"
#include "Engine/FatalEngineException.h"
#include "Engine/Locale/Locale.h"
#include "Model/GameConfig.h"
#include "Model/GameSettings.h"
#include "Model/GameManager.h"
#include "Model/World/WorldStateMachine.h"
#include "GameKernel.h"

namespace LittleEngine
{
bool GameKernel::Boot(const GameConfig& config)
{
	GameSettings* pSettings = GameSettings::Instance();
	m_gfx.m_uiSpace = config.UISpace();
	m_gfx.m_viewportHeight = ToS32(pSettings->ViewportHeight());
	m_gfx.SetWorldHeight(config.WorldHeight(), true);
#ifdef DEBUGGING
	m_gfx.m_overrideNativeAR = config.ForceViewportAR();
#endif
	m_gfx.Init();
	LEContext::Data data;
	data.viewportData.viewportSize = pSettings->SafeGetViewportSize();
	data.viewportData.title = LOC(config.TitleBarText());
	data.viewportData.style = pSettings->GetViewportStyle();
	data.tickRate = config.TickRate();
	data.bRenderThread = config.m_bRenderThread;
	data.renderThreadStartDelay = config.RenderThreadStartDelay();
	Core::Property::Persistor inputMapPersistor;
	auto pInputMapFile = pSettings->GetValue("CUSTOM_INPUT_MAP");
	if (pInputMapFile)
	{
		std::string inputMapFile = OS::Env()->FullPath(pInputMapFile->c_str());
		if (inputMapPersistor.Load(inputMapFile))
		{
			u16 count = data.inputMap.Import(inputMapPersistor);
			if (count > 0)
			{
				LOG_I("[GameKernel] Loaded %u custom Input Mappings successfully", count);
			}
		}
	}
	try
	{
		m_uContext = MakeUnique<LEContext>(std::move(data));
		m_uWSM = MakeUnique<WorldStateMachine>(*m_uContext);
		m_uGame = MakeUnique<GameManager>(*m_uContext, *m_uWSM);
	}
	catch (const FatalEngineException& e)
	{
		LOG_E("[GameKernel] Error creating GL context!\n%s", e.what());
		return false;
	}
	return true;
}

void GameKernel::Shutdown()
{
	m_uWSM = nullptr;
	m_uGame = nullptr;
	m_uContext = nullptr;
}

void GameKernel::Start(std::string coreManifestID /* = "" */, std::string gameStyleID /* = "" */, Task onManifestLoaded /* = nullptr */)
{
	Assert(m_uContext, "Context is null!");
	m_uWSM->Start(std::move(coreManifestID), std::move(gameStyleID), std::move(onManifestLoaded));
	m_uContext->SetPointerPosition(Vector2::Zero, false);
}

void GameKernel::Step(Time fdt)
{
	m_uGame->Step(fdt);
}

void GameKernel::Tick(Time dt)
{
	m_uGame->Tick(dt);
}

LEContext* GameKernel::Context() const
{
	return m_uContext.get();
}
} // namespace LittleEngine
