#include "stdafx.h"
#include "Core/Utils.h"
#include "Core/ArchiveReader.h"
#include "SFMLAPI/System/SFAssets.h"
#include "LittleEngine/Context/LEContext.h"
#include "LittleEngine/Renderer/LERenderer.h"
#include "LittleEngine/Repository/LERepository.h"
#include "LEGame/Model/GameManager.h"
#include "LEGame/Model/GameSettings.h"
#include "LEGame/Model/World/WorldStateMachine.h"
#include "LEGame/Model/UI/UIElement.h"
#include "LEGame/Framework/UI/Widgets/UIButton.h"
#include "LEGame/Framework/UI/Widgets/UISelection.h"
#include "LEGame/Framework/UI/Widgets/UIToggle.h"
#include "UIOptions.h"

namespace LittleEngine
{
UIOptions::UIOptions() = default;
UIOptions::~UIOptions() = default;

void UIOptions::OnCreated()
{
	UIContainer::OnCreated();

	Deserialise(g_pRepository->Load<TextAsset>("Texts/Layouts/UIOptions.uil")->GetText());

	auto pWorldSelection = Find<UISelection>("world_selection");
	auto stateIDs = g_pGameManager->GetAllWorldIDs();
	Core::Remove(stateIDs, g_pGameManager->GetActiveWorldID());
	pWorldSelection->SetOptions(Strings::ToString(stateIDs, "World "));
	m_tokens.push_back(pWorldSelection->RegisterOnChanged([&, stateIDs](std::pair<size_t, String> selected) {
		g_pGameManager->LoadWorld(stateIDs[selected.first]);
	}));

	auto pHelpButton = Find<UIButton>("help_button");
	m_tokens.push_back(pHelpButton->AddCallback([]() { LOG_D("Not Implemented"); }));

	auto pQuitButton = Find<UIButton>("quit_button");
	m_tokens.push_back(pQuitButton->AddCallback([&]() { g_pGameManager->Quit(); }));

	auto pResolutionSelection = Find<UISelection>("resolution_selection");
	const auto& resolutions = g_pGameManager->Renderer()->GetValidViewportSizes();
	Vec<u32> resolutionHeights;
	std::for_each(resolutions.begin(), resolutions.end(), [&resolutionHeights](const auto& kvp) {
		resolutionHeights.push_back(kvp.second.height);
	});
	pResolutionSelection->SetOptions(Strings::ToString(resolutionHeights, "", "p"));
	pResolutionSelection->GetRoot()->m_transform.nPosition = {Fixed::OneHalf, Fixed::OneHalf};
	m_tokens.push_back(pResolutionSelection->RegisterOnChanged([&, resolutionHeights](std::pair<size_t, String> kvp) {
		if (g_pGameManager->Context()->TrySetViewportSize(resolutionHeights[kvp.first]))
		{
			GameSettings::Instance()->SetViewportHeight(resolutionHeights[kvp.first]);
		}
	}));

	auto pBorderlessToggle = Find<UIToggle>("borderless_toggle");
	pBorderlessToggle->SetOn(GameSettings::Instance()->IsBorderless());
	m_tokens.push_back(pBorderlessToggle->AddCallback([&](bool bBorderless) {
		GameSettings::Instance()->SetBorderless(bBorderless);
		g_pGameManager->Context()->SetWindowStyle(bBorderless ? SFViewportStyle::Bordlerless
															  : SFViewportStyle::Default);
	}));

	auto pCloseButton = Find<UIButton>("close_button");
	m_tokens.push_back(pCloseButton->AddCallback([&]() { Destruct(); }));
}
} // namespace LittleEngine