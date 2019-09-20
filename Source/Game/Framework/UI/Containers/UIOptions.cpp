#include "Core/Utils.h"
#include "Core/ArchiveReader.h"
#include "SFMLAPI/System/Assets.h"
#include "Engine/GFX.h"
#include "Engine/Context/LEContext.h"
#include "Engine/Physics/Collider.h"
#include "Engine/Renderer/LERenderer.h"
#include "Engine/Repository/LERepository.h"
#include "Model/GameManager.h"
#include "Model/GameSettings.h"
#include "Model/World/Entity.h"
#include "Model/World/WorldStateMachine.h"
#include "Model/UI/UIElement.h"
#include "Model/UI/UIManager.h"
#include "Framework/Components/ControllerComponent.h"
#include "Framework/Utility/Debug/RenderStatsRenderer.h"
#include "Framework/Utility/Debug/DebugProfiler.h"
#include "Framework/UI/Contexts/UIButtonDrawer.h"
#include "Framework/UI/Widgets/UIButton.h"
#include "Framework/UI/Widgets/UISelection.h"
#include "Framework/UI/Widgets/UIToggle.h"
#include "UIOptions.h"

namespace LittleEngine
{
UIOptions::UIOptions() = default;
UIOptions::~UIOptions() = default;

void UIOptions::OnCreated()
{
	Deserialise(g_pRepository->Load<TextAsset>("Texts/Layouts/UIOptions.uil")->Text());

	auto pHelpButton = Find<UIButton>("help_button");
	if (pHelpButton)
	{
		m_tokens.push_back(pHelpButton->AddCallback([]() { LOG_D("Not Implemented"); }));
	}

	auto pQuitButton = Find<UIButton>("quit_button");
	if (pQuitButton)
	{
		m_tokens.push_back(pQuitButton->AddCallback([&]() { g_pGameManager->Quit(); }));
	}

	auto pResolutionSelection = Find<UISelection>("resolution_selection");
	if (pResolutionSelection)
	{
		const auto& resolutions = g_pGFX->ValidViewportSizes();
		Vec<u32> resolutionHeights;
		std::for_each(resolutions.begin(), resolutions.end(),
					  [&resolutionHeights](const auto& kvp) { resolutionHeights.push_back(kvp.second.height); });
		pResolutionSelection->SetOptions(Strings::ToString(resolutionHeights, "", "p"));
		m_tokens.push_back(pResolutionSelection->RegisterOnChanged([&, resolutionHeights](Pair<size_t, String> kvp) {
			if (g_pGameManager->Context()->TrySetViewportSize(resolutionHeights[kvp.first]))
			{
				GameSettings::Instance()->SetViewportHeight(resolutionHeights[kvp.first]);
			}
		}));
	}

	auto pBorderlessToggle = Find<UIToggle>("borderless_toggle");
	if (pBorderlessToggle)
	{
		pBorderlessToggle->SetOn(GameSettings::Instance()->IsBorderless());
		m_tokens.push_back(pBorderlessToggle->AddCallback([&](bool bBorderless) {
			GameSettings::Instance()->SetBorderless(bBorderless);
			g_pGameManager->Context()->SetWindowStyle(bBorderless ? ViewportStyle::Bordlerless : ViewportStyle::Default);
		}));
	}

	auto pCloseButton = Find<UIButton>("close_button");
	if (pCloseButton)
	{
		m_tokens.push_back(pCloseButton->AddCallback([&]() { Destruct(); }));
	}

#if defined(DEBUGGING)
	auto pDebugButton = Find<UIButton>("debug_button");
	if (pDebugButton)
	{
		m_tokens.push_back(pDebugButton->AddCallback([&]() {
			auto pDrawer = g_pGameManager->UI()->PushContext<UIButtonDrawer>("DebugButtonDrawer");
			if (pDrawer)
			{
				pDrawer->m_bAutoDestroyOnCancel = true;

				UIStyle drawerStyle;
				drawerStyle.size = {600, 700};
				drawerStyle.fill = Colour(200, 200, 200, 240);
				drawerStyle.border = 4;
				drawerStyle.outline = Colour::Magenta;
				pDrawer->SetBaseStyle(std::move(drawerStyle), {-Fixed::OneHalf, Fixed::Zero});

#if ENABLED(PROFILER)
				m_tokens.push_back(pDrawer->AddButton("Profiler", []() { Debug::Profiler::Toggle(); }));
#endif
#if ENABLED(RENDER_STATS)
				m_tokens.push_back(pDrawer->AddButton("RenderStats", []() {
					Debug::RenderStatsRenderer::s_bConsoleRenderStatsEnabled = !Debug::RenderStatsRenderer::s_bConsoleRenderStatsEnabled;
				}));
#endif
				m_tokens.push_back(pDrawer->AddButton("Colliders", []() { Collider::s_bShowDebugShape = !Collider::s_bShowDebugShape; }));
				m_tokens.push_back(
					pDrawer->AddButton("Orientation Vectors", []() { Entity::s_bShowOrientation = !Entity::s_bShowOrientation; }));

				m_tokens.push_back(pDrawer->AddButton("Controller Orientation", []() {
					ControllerComponent::s_bShowJoystickOrientation = !ControllerComponent::s_bShowJoystickOrientation;
				}));

				pDrawer->ResetSelection();
			}
		}));
	}
#endif
}

void UIOptions::OnActivated()
{
	auto pWorldSelection = Find<UISelection>("world_selection");
	if (pWorldSelection)
	{
		auto stateIDs = g_pGameManager->AllWorldIDs();
		Core::Remove(stateIDs, g_pGameManager->ActiveWorldID());
		pWorldSelection->SetOptions(Strings::ToString(stateIDs, "World "));
		pWorldSelection->SetText("Load World");
		m_worldSelectToken = pWorldSelection->RegisterOnChanged(
			[&, stateIDs](Pair<size_t, String> selected) { g_pGameManager->LoadWorld(stateIDs[selected.first]); });
	}

	auto pResolutionSelection = Find<UISelection>("resolution_selection");
	if (pResolutionSelection)
	{
		pResolutionSelection->SetText("Resolution");
	}

	auto pBorderlessToggle = Find<UIToggle>("borderless_toggle");
	if (pBorderlessToggle)
	{
		pBorderlessToggle->SetOn(GameSettings::Instance()->IsBorderless());
	}
}
} // namespace LittleEngine
