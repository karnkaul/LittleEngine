#include "stdafx.h"
#include "BootWorld.h"
#include "GameFramework/GameFramework.h"

namespace LittleEngine
{
namespace
{
UIButtonDrawerData GetDrawerData()
{
	UIButtonData* pCustomButton = nullptr;
	return UIButtonDrawerData::CreateDrawer(true, {600, 500}, Colour::Transparent, pCustomButton);
}

// Colour start;
// Colour end;
// SFPrimitive* pP0, *pP1, *pP2;
//
// void CreatePrimitives() {
//	SFPrimitiveData d0;
//	d0.primary = Colour::Magenta;
//	pP0 = Services::RHeap()->New(d0);
//	pP0->SetSize({ 100, 100 }, SFShapeType::Rectangle);

//	SFPrimitiveData d1;
//	TextureAsset* pTexture = Services::Engine()->Repository()->Load<TextureAsset>("Ship.png");
//	d1.texture = pTexture;
//	pP1 = Services::RHeap()->New(d1);
//	if (pP1) pP1->SetEnabled(true);

//	SFPrimitiveData d2;
//	d2.font = Services::Engine()->Repository()->GetDefaultFont();
//	d2.text = "Hello WORLD!";
//	pP2 = Services::RHeap()->New(d2);
//	pP2->SetTextSize(50)->SetLayer(LAYER_UI)->SetPosition({ 0, 200 });
//}

// void DestroyPrimitives() {
//	Services::RHeap()->Destroy({ pP0, pP1, pP2 });
//	pP0 = nullptr;
//	pP1 = nullptr;
//	pP2 = nullptr;
//}

// BootWorld* pBootWorld = nullptr;
// EngineInput::Token token0, token1;
///*bool OnBackPressed(const EngineInput::Frame& frame) {
//	if (frame.IsReleased(GameInputType::Back)) {
//		Services::Engine()->Terminate();
//		return true;
//	}
//	else if (frame.IsReleased(GameInputType::Enter) && pBootWorld) {
//		bool bSuccess = pBootWorld->LoadWorld(1);
//		if (bSuccess) pBootWorld = nullptr;
//	}
//	return false;
//}*/

// Vector2 inputAxes;
// bool OnInput1(const EngineInput::Frame& frame) {
//	if (frame.IsHeld(GameInputType::Left)) {
//		inputAxes.x = -10;
//	}
//	else if (frame.IsHeld(GameInputType::Right)) {
//		inputAxes.x = 10;
//	}

//	if (frame.IsReleased(GameInputType::Back)) {
//		LogD("GameInputType::Back Released detected! Releasing token");
//		token1 = nullptr;
//		return true;
//	}
//	else {
//		LogD("GameInputType::Back Released not detected. Keeping blocking token");
//	}
//	return false;
//}
} // namespace

BootWorld::BootWorld() : World("Boot")
{
}

void BootWorld::OnActivated()
{
	m_pLogoFont = Repository()->Load<FontAsset>("Sunscreen.otf");
	if ((m_pLogoDrawer = Game()->UI()->SpawnContext<UIButtonDrawer>()))
	{
		m_pLogoDrawer->InitButtonDrawer(GetDrawerData());
		m_pLogoHeader = m_pLogoDrawer->AddElement<UIElement>("Logo Header");
		m_pLogoHeader->SetText(UIText("Little Engine", 80, Colour::White));
		if (m_pLogoFont)
			m_pLogoHeader->SetFont(*m_pLogoFont);
		m_pLogoHeader->m_transform.UnsetParent();
		m_pLogoHeader->m_transform.nPosition = {0, Fixed(0.8f)};
		m_tokenHandler.AddToken(m_pLogoDrawer->AddButton("Start", std::bind(&BootWorld::OnLoadNextWorld, this)));
		m_tokenHandler.AddToken(m_pLogoDrawer->AddButton("Quit", std::bind(&World::Quit, this)));
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
		if (m_pLogoHeader)
		{
			Colour colour = m_pLogoHeader->GetText()->GetPrimaryColour();
			colour.a = UByte(alpha.ToU32());
			m_pLogoHeader->GetText()->SetPrimaryColour(colour);
		}
	}
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
