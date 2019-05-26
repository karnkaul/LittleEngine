#include "stdafx.h"
#include "Core/Utils.h"
#include "Core/ArchiveReader.h"
#include "SFMLAPI/System/SFAssets.h"
#include "LittleEngine/Engine/EngineService.h"
#include "LittleEngine/GFX/GFX.h"
#include "LittleEngine/Services/Services.h"
#include "LittleEngine/Game/GameManager.h"
#include "LittleEngine/Repository/EngineRepository.h"
#include "LittleEngine/Game/World/WorldStateMachine.h"
#include "LittleEngine/UI/UIElement.h"
#include "GameFramework/UI/Widgets/UIButton.h"
#include "GameFramework/UI/Widgets/UISelection.h"
#include "GameFramework/UI/Widgets/UIToggle.h"
#include "UIOptions.h"

namespace LittleEngine
{
UIOptions::UIOptions() = default;
UIOptions::~UIOptions() = default;

void UIOptions::OnCreated()
{
	UIContainer::OnCreated();

	Deserialise(Services::Game()->Repository()->Load<TextAsset>("Texts/Layouts/UIOptions.uil")->GetText());

	auto pWorldSelection = Find<UISelection>("world_selection");
	auto stateIDs = Services::Game()->Worlds()->GetAllStateIDs();
	Core::Remove(stateIDs, Services::Game()->Worlds()->GetActiveStateID());
	pWorldSelection->SetOptions(Strings::ToString(stateIDs, "World "));
	m_tokens.push_back(pWorldSelection->RegisterOnChanged([stateIDs](std::pair<size_t, String> selected) {
		Services::Game()->Worlds()->LoadState(stateIDs[selected.first]);
	}));

	auto pHelpButton = Find<UIButton>("help_button");
	m_tokens.push_back(pHelpButton->AddCallback([]() { LOG_D("Not Implemented"); }));

	auto pQuitButton = Find<UIButton>("quit_button");
	m_tokens.push_back(pQuitButton->AddCallback([]() { Services::Engine()->Terminate(); }));

	auto pResolutionSelection = Find<UISelection>("resolution_selection");
	const auto& resolutions = GFX::GetValidWindowSizes();
	Vec<u32> resolutionHeights;
	std::for_each(resolutions.begin(), resolutions.end(), [&resolutionHeights](const auto& kvp) {
		resolutionHeights.push_back(kvp.second.height);
	});
	pResolutionSelection->SetOptions(Strings::ToString(resolutionHeights, "", "p"));
	pResolutionSelection->GetRoot()->m_transform.nPosition = {Fixed::OneHalf, Fixed::OneHalf};
	m_tokens.push_back(pResolutionSelection->RegisterOnChanged([resolutionHeights](std::pair<size_t, String> kvp) {
		Services::Engine()->TrySetWindowSize(resolutionHeights[kvp.first]);
	}));

	auto pBorderlessToggle = Find<UIToggle>("borderless_toggle");
	m_tokens.push_back(pBorderlessToggle->AddCallback([](bool bBorderless) {
		Services::Engine()->SetWindowStyle(bBorderless ? SFWindowStyle::Bordlerless : SFWindowStyle::Default);
	}));

	auto pCloseButton = Find<UIButton>("close_button");
	m_tokens.push_back(pCloseButton->AddCallback([&]() { Destruct(); }));
}
} // namespace LittleEngine