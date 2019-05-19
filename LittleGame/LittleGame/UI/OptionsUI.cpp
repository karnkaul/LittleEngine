#include "stdafx.h"
#include "GameFramework/GameFramework.h"
#include "OptionsUI.h"

namespace LittleEngine
{
bool OptionsUI::s_bShowQuitButton = true;

void OptionsUI::OnCreated()
{
	m_data.size = {800, 600};
	UIDialogue::OnCreated();

	SetHeader("Options");

	// Load World
	auto stateIDs = Services::Game()->Worlds()->GetAllStateIDs();
	auto thisStateID = Services::Game()->Worlds()->GetActiveStateID();
	Core::VectorErase(stateIDs, thisStateID);
	auto pLoadWorldSelection = AddWidget<UISelection>("LoadWorld");
	pLoadWorldSelection->SetText("Load World");
	pLoadWorldSelection->AddOptions(Strings::ToString(stateIDs, "World "));
	m_tokens.push_back(pLoadWorldSelection->RegisterOnChanged([stateIDs](std::pair<size_t, String> selected) {
		Services::Game()->Worlds()->LoadState(stateIDs[selected.first]);
	}));
	pLoadWorldSelection->GetRoot()->m_transform.nPosition = {-Fixed::OneHalf, Fixed::OneHalf};

	// Help
	auto pHelpButton = AddWidget<UIButton>("Help");
	pHelpButton->SetText("Help");
	pHelpButton->GetRoot()->m_transform.nPosition = {-Fixed::OneHalf, Fixed::Zero};
	m_tokens.push_back(pHelpButton->AddCallback([]() { LOG_D("Not Implemented"); }));

	// Quit
	if (s_bShowQuitButton)
	{
		auto pQuitButton = AddWidget<UIButton>("Quit");
		pQuitButton->SetText("Quit");
		pQuitButton->GetRoot()->m_transform.nPosition = {-Fixed::OneHalf, -Fixed::OneHalf};
		m_tokens.push_back(pQuitButton->AddCallback([]() { Services::Engine()->Terminate(); }));
	}

	// Last UIWidget on this column
	m_tokens.push_back(AddMainButton("Close", [&]() { Destruct(); }, true));

	// Resolutions
	const auto& resolutions = GFX::GetValidWindowSizes();
	Vec<u32> resolutionHeights(resolutions.size(), 0);
	size_t idx = 0;
	for (const auto& kvp : resolutions)
	{
		resolutionHeights[idx++] = kvp.second.height;
	}
	auto pResolutionSelection = AddWidget<UISelection>("Resolution", nullptr, true);
	pResolutionSelection->SetText("Resolution");
	pResolutionSelection->AddOptions(Strings::ToString(resolutionHeights, "", "p"));
	pResolutionSelection->GetRoot()->m_transform.nPosition = {Fixed::OneHalf, Fixed::OneHalf};
	m_tokens.push_back(pResolutionSelection->RegisterOnChanged([resolutionHeights](std::pair<size_t, String> kvp) {
		Services::Engine()->TrySetWindowSize(resolutionHeights[kvp.first]);
	}));

	// Borderless
	auto pBorderlessToggle = AddWidget<UIToggle>("Borderless");
	m_tokens.push_back(pBorderlessToggle->AddCallback([&](bool bBorderless) {
		Services::Engine()->SetWindowStyle(bBorderless ? SFWindowStyle::Bordlerless : SFWindowStyle::Default);
	}));
	pBorderlessToggle->GetRoot()->m_transform.nPosition = {Fixed::OneHalf, Fixed::Zero};
	pBorderlessToggle->SetOffColour(Colour(150, 150, 150, 255));
	pBorderlessToggle->SetOn(GameSettings::Instance()->IsBorderless());
	pBorderlessToggle->SetText("Borderless");
}
} // namespace LittleEngine