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

	Deserialise(Services::Game()->Repository()->Load<TextAsset>("Layouts/UIOptions.uil")->GetText());

	auto pWorldSelection = Get<UISelection>("world_selection");
	auto stateIDs = Services::Game()->Worlds()->GetAllStateIDs();
	auto thisStateID = Services::Game()->Worlds()->GetActiveStateID();
	Core::VectorErase(stateIDs, thisStateID);
	pWorldSelection->SetOptions(Strings::ToString(stateIDs, "World "));
	m_tokens.push_back(pWorldSelection->RegisterOnChanged([stateIDs](std::pair<size_t, String> selected) {
		Services::Game()->Worlds()->LoadState(stateIDs[selected.first]);
	}));

	auto pHelpButton = Get<UIButton>("help_button");
	m_tokens.push_back(pHelpButton->AddCallback([]() { LOG_D("Not Implemented"); }));

	auto pQuitButton = Get<UIButton>("quit_button");
	m_tokens.push_back(pQuitButton->AddCallback([]() { Services::Engine()->Terminate(); }));

	auto pResolutionSelection = Get<UISelection>("resolution_selection");
	const auto& resolutions = GFX::GetValidWindowSizes();
	Vec<u32> resolutionHeights(resolutions.size(), 0);
	size_t idx = 0;
	for (const auto& kvp : resolutions)
	{
		resolutionHeights[idx++] = kvp.second.height;
	}
	pResolutionSelection->SetOptions(Strings::ToString(resolutionHeights, "", "p"));
	pResolutionSelection->GetRoot()->m_transform.nPosition = {Fixed::OneHalf, Fixed::OneHalf};
	m_tokens.push_back(pResolutionSelection->RegisterOnChanged([resolutionHeights](std::pair<size_t, String> kvp) {
		Services::Engine()->TrySetWindowSize(resolutionHeights[kvp.first]);
	}));

	auto pBorderlessToggle = Get<UIToggle>("borderless_toggle");
	m_tokens.push_back(pBorderlessToggle->AddCallback([](bool bBorderless) {
		Services::Engine()->SetWindowStyle(bBorderless ? SFWindowStyle::Bordlerless : SFWindowStyle::Default);
	}));

	auto pCloseButton = Get<UIButton>("close_button");
	m_tokens.push_back(pCloseButton->AddCallback([&]() { Destruct(); }));
}
} // namespace LittleEngine