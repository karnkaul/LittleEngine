#include "stdafx.h"
#include "ArchiveReader.h"
#include "LittleEngine/Debug/Console/Tweakable.h"
#include "GameFramework/GameFramework.h"
#include "TestWorld.h"
#include "UI/OptionsUI.h"

// TODO: Remove
#include "LittleEngine/RenderLoop/RenderFactory.h"
#include "SFMLAPI/Windowing/SFWindow.h"
#include "SFMLAPI/System/SFTypes.h"

namespace LittleEngine
{
namespace
{
bool bSpawnColliderMinefield = false;
EngineInput::Token t0;
TestWorld* pTestWorld = nullptr;

Entity *pEntity0 = nullptr, *pEntity1 = nullptr;
Entity *pEntity2 = nullptr, *pEntity3 = nullptr;
Entity* pEntity4 = nullptr;

void OnEnter()
{
	if (!pEntity2 && !pEntity3)
	{
		pEntity2 = pTestWorld->Game()->NewEntity<Entity>("Yellow Circle", Vector2(-300, 300));
		if (pEntity2)
		{
			auto rc0 = pEntity2->AddComponent<RenderComponent>();
			rc0->SetShape(LAYER_DEFAULT)->m_pSFPrimitive->SetSize({100, 100}, SFShapeType::Circle)->SetPrimaryColour(Colour::Yellow);
			auto t0 = pEntity2->AddComponent<CollisionComponent>();
			t0->AddCircle(100);
		}

		pEntity3 = pTestWorld->Game()->NewEntity<Entity>("Blue Rectangle", Vector2(500, -200));
		if (pEntity3)
		{
			auto rc1 = pEntity3->AddComponent<RenderComponent>();
			rc1->SetShape(LAYER_DEFAULT)->m_pSFPrimitive->SetSize({600, 100}, SFShapeType::Rectangle)->SetPrimaryColour(Colour::Blue);
			auto t1 = pEntity3->AddComponent<CollisionComponent>();
			t1->AddAABB(AABBData({600, 100}));
		}
	}
	else
	{
		pEntity2->Destruct();
		pEntity2 = nullptr;
		pEntity3->Destruct();
		pEntity3 = nullptr;
	}

	if (!pEntity4)
	{
		pEntity4 = pTestWorld->Game()->NewEntity<Entity>("SpriteSheetTest");
		pEntity4->m_transform.localPosition = {-200, -200};
		auto rc4 = pEntity4->AddComponent<RenderComponent>();
		rc4->SetSpriteSheet(SpriteSheet("Textures/TestSheet_64x64_6x6", Time::Seconds(1.0f)), LAYER_FX);
		//rc4->SetSpriteSheet(SpriteSheet(std::move(data), pTexture, Time::Seconds(1.0f)));
	}
	else
	{
		pEntity4->Destruct();
		pEntity4 = nullptr;
	}
}

//bool bLoopingPS = false;
ParticleSystem* pParticleSystem0 = nullptr;
void OnSelect()
{
	if (!pParticleSystem0)
	{
		return;
	}

	if (pParticleSystem0->IsPlaying())
	{
		pParticleSystem0->Stop();
	}
	else
	{
		// pParticleSystem0->SetEnabled(true);
		//Fixed x = Maths::Random::Range(-Fixed::One, Fixed::One);
		//Fixed y = Maths::Random::Range(-Fixed::One, Fixed::One);
		//Vector2 worldPos = GFX::Project({x, y}, false);
		//pParticleSystem0->m_transform.localPosition = worldPos;
		pParticleSystem0->Start();
		//pTestWorld->Game()->WorldCamera()->Shake();
	}
}

Player* pPlayer = nullptr;
bool bParented = false;
void OnX()
{
	if (pEntity0 && pPlayer)
	{
		if (!bParented)
		{
			pEntity0->m_transform.SetParent(pPlayer->m_transform);
			bParented = true;
		}
		else
		{
			pEntity0->m_transform.UnsetParent();
			bParented = false;
		}
	}
}
void OnY()
{
	if (pEntity0 && pPlayer)
	{
		if (!bParented)
		{
			pEntity0->m_transform.SetParent(pPlayer->m_transform, false);
			bParented = true;
		}
		else
		{
			pEntity0->m_transform.UnsetParent(false);
			bParented = false;
		}
	}
}

bool Test_OnInput(const EngineInput::Frame& frame)
{
	if (frame.IsReleased(GameInputType::Enter))
	{
		OnEnter();
	}

	if (frame.IsReleased(GameInputType::Select))
	{
		OnSelect();
	}

	if (frame.IsReleased(GameInputType::X))
	{
		OnX();
	}

	if (frame.IsReleased(GameInputType::Y))
	{
		OnY();
	}

	if (frame.IsReleased(GameInputType::LB))
	{
		Assert(false, "Test Assert");
	}

	if (frame.IsHeld(GameInputType::Left) && frame.IsHeld(GameInputType::RB))
	{
		pTestWorld->Game()->WorldCamera()->m_transform.localPosition.x -= Fixed::Three;
		return true;
	}

	if (frame.IsHeld(GameInputType::Right) && frame.IsHeld(GameInputType::RB))
	{
		pTestWorld->Game()->WorldCamera()->m_transform.localPosition.x += Fixed::Three;
		return true;
	}
	return false;
}

void SpawnColliderMinefield()
{
	u32 id = 0;
	for (Fixed x = -600; x < 800; x += 200)
	{
		for (Fixed y = -350; y < 500; y += 200)
		{
			String name = "ColliderMine_" + Strings::ToString(id++);
			auto* pE = pTestWorld->Game()->NewEntity<Entity>(name, Vector2(x, y));
			auto* pCC = pE->AddComponent<CollisionComponent>();
			pCC->AddAABB(AABBData({100, 100}));
		}
	}
}

sf::VertexArray va(sf::Quads, 4);
void InitVertexArrayTests()
{
	va[0].position = Cast(SFPrimitive::WorldToScreen(Vector2(-100, 100)));
	va[1].position = Cast(SFPrimitive::WorldToScreen(Vector2(100, 100)));
	va[2].position = Cast(SFPrimitive::WorldToScreen(Vector2(100, -100)));
	va[3].position = Cast(SFPrimitive::WorldToScreen(Vector2(-100, -100)));

	va[0].color = Cast(Colour(200, 255, 100, 255));
	va[1].color = Cast(Colour(200, 255, 100, 255));
	va[2].color = Cast(Colour(200, 255, 100, 255));
	va[3].color = Cast(Colour(200, 255, 100, 255));

	//Services::RFactory()->m_vertArrs.emplace_back(va);
}

//TweakBool(test0, nullptr);
//TweakBool(testLongAssNameTweakable0, nullptr);
//TweakBool(test1, nullptr);
//TweakS32(test2, nullptr);
//TweakF32(testLongAssNameTweakable1, nullptr);
//TweakString(test3, nullptr);
void StartTests()
{
	pEntity0 = pTestWorld->Game()->NewEntity<Entity>("Entity0", {300, 200});
	auto rc0 = pEntity0->AddComponent<RenderComponent>();
	rc0->SetShape(LAYER_DEFAULT)->m_pSFPrimitive->SetSize({300, 100}, SFShapeType::Rectangle)
		->SetPrimaryColour(Colour::Cyan)
		->SetEnabled(true);

	pEntity1 = pTestWorld->Game()->NewEntity<Entity>("Entity1", GFX::Project({0, Fixed(0.9f)}, false));
	auto rc1 = pEntity1->AddComponent<RenderComponent>();
	FontAsset* font = pTestWorld->Game()->Repository()->GetDefaultFont();
	rc1->SetShape(LAYER_DEFAULT)->m_pSFPrimitive->SetText("Hello World!")
		->SetFont(*font)
		->SetTextSize(50)
		->SetPrimaryColour(Colour(200, 150, 50))
		->SetEnabled(true);

	pPlayer = pTestWorld->Game()->NewEntity<Player>("Player0", {0, -300});
	auto* pTexture = pTestWorld->Game()->Repository()->Load<TextureAsset>("Textures/Ship.png");
	PlayerData data(
		*pTexture, {PlayerCollider(AABBData({120, 60}), {0, -15}), PlayerCollider(AABBData({60, 80}))});
	pPlayer->InitPlayer(std::move(data));

	//String path = bLoopingPS ? "VFX/Fire0/Fire0_loop.psdata.min" : "VFX/Fire0/Fire0_noloop.psdata.min";
	String path = !OS::IsSHIPPING() && OS::IsDebuggerAttached() ? "VFX/Stars0/Stars0.psdata" : "VFX/Stars0/Stars0.psdata.min"; 
	auto* pText = pTestWorld->Repository()->Load<TextAsset>(path);
	GData psGData(pText->GetText());
	pParticleSystem0 = pTestWorld->Game()->NewEntity<ParticleSystem>("Fire0");
	pParticleSystem0->InitParticleSystem(ParticleSystemData(psGData));
	//pParticleSystem0->m_transform.localScale = {Fixed(2.0f), Fixed(2.0f)};
	pParticleSystem0->Stop();

	if (bSpawnColliderMinefield)
	{
		SpawnColliderMinefield();
	}

	InitVertexArrayTests();
}

UIButtonDrawer* pButtonDrawer = nullptr;
bool bModal = true;
bool bSpawnedDrawer = false;
Vec<EngineInput::Token> debugTokens;

UIDialogue* pDialogue = nullptr;

// Fixed progress;
UPtr<UIProgressBar> uProgressBar = nullptr;
UPtr<UIElement> uProgressBG = nullptr;
Time elapsed = Time::Zero;

void SpawnDialogue()
{
	pDialogue = pTestWorld->Game()->UI()->PushContext<UIDialogue>("TestUIDialogue");
	pDialogue->SetHeader(UIText("Title", 25, Colour::Black));
	pDialogue->SetContent(UIText("Content goes here", 15, Colour::Black), &Colour::White);
	debugTokens.push_back(pDialogue->AddMainButton("OK", []() { LOG_D("OK pressed!"); }, false));
	debugTokens.push_back(pDialogue->AddOtherButton("Cancel", []() { pDialogue->Destruct(); }, false));
	pDialogue->SetActive(true);
}

void SpawnToggle()
{
	Fixed x = 300;
	Fixed y = 200;
	auto* pParent = pTestWorld->Game()->UI()->PushContext<UIContext>("TestToggleUIC");
	pParent->GetRootElement()->m_transform.size = {x, y};
	UIWidgetStyle toggleStyle = UIWidgetStyle::GetDefault0();
	toggleStyle.widgetSize = {x, y * Fixed::OneHalf};
	toggleStyle.background = Colour::Yellow;
	auto* pToggle0 = pParent->AddWidget<UIToggle>("Toggle0", &toggleStyle);
	auto* pToggle1 = pParent->AddWidget<UIToggle>("Toggle1", &toggleStyle);

	debugTokens.push_back(pToggle0->AddCallback([](bool bVal) { LOG_W("Toggle0 changed! %d", bVal); }));
	pToggle0->GetRoot()->m_transform.bAutoPad = true;
	pToggle0->SetText("Toggle 0")->SetOn(false)->GetRoot()->m_transform.nPosition = {0, 1};

	debugTokens.push_back(
		pToggle1->AddCallback([](bool bValue) { LOG_W("Toggle1 changed! %d", bValue); }));
	pToggle1->GetRoot()->m_transform.bAutoPad = true;
	pToggle1->SetText("Toggle 1")->SetOn(true)->GetRoot()->m_transform.nPosition = {0, -1};

	pParent->m_bAutoDestroyOnCancel = true;
	pParent->SetActive(true);
}

void TestTick(Time dt)
{
	elapsed += dt;
	if (elapsed.AsSeconds() >= 1 && !bSpawnedDrawer)
	{
		bSpawnedDrawer = true;
		pButtonDrawer = pTestWorld->Game()->UI()->PushContext<UIButtonDrawer>("TestUIButtonDrawer");
		pButtonDrawer->m_bAutoDestroyOnCancel = !bModal;
		UIStyle panelStyle;
		panelStyle.size = {500, 600};
		panelStyle.fill = Colour(100, 100, 100, 100);
		pButtonDrawer->SetPanel(panelStyle);
		UIButton* pButton1 = nullptr;
		debugTokens.push_back(
			pButtonDrawer->AddButton("Button 0", []() { LOG_D("Button 0 pressed!"); }));
		debugTokens.push_back(pButtonDrawer->AddButton(
			"Options", []() { Services::Game()->UI()->PushContext<OptionsUI>("Options"); }, &pButton1));
		pButton1->SetInteractable(false);
		debugTokens.push_back(pButtonDrawer->AddButton("Toggle B1", [pButton1]() {
			pButton1->SetInteractable(!pButton1->IsInteractable());
			LOG_D("Button 1 Set Interactable: %s", Strings::ToString(pButton1->IsInteractable()).c_str());
		}));
		/*debugTokens.push_back(
			pButtonDrawer->AddButton("Button 3", []() { LOG_D("Button 3 pressed!"); }));*/
		debugTokens.push_back(pButtonDrawer->AddButton("Toggle", &SpawnToggle));
		debugTokens.push_back(pButtonDrawer->AddButton("Dialogue", []() { SpawnDialogue(); }));
		if (bModal)
		{
			debugTokens.push_back(
				pButtonDrawer->AddButton("Cancel", []() { pButtonDrawer->Destruct(); }));
		}
		pButtonDrawer->SetActive(true);
	}

	static bool bSpawnedTextInput = false;
	static UIContext* pTextContext = nullptr;
	static UITextInput* pTextInput = nullptr;
	if (elapsed.AsSeconds() >= 2 && !bSpawnedTextInput)
	{
		pTextContext = pTestWorld->Game()->UI()->PushContext<UIContext>("TestTextInputUIC");
		pTextInput = pTextContext->AddWidget<UITextInput>("TextInput");
		pTextContext->m_bAutoDestroyOnCancel = true;
		pTextContext->SetActive(true);
		bSpawnedTextInput = true;
	}
}

void Cleanup()
{
	elapsed = Time::Zero;
	pTestWorld = nullptr;

	pParticleSystem0 = nullptr;
	pPlayer = nullptr;

	pEntity0 = pEntity1 = pEntity2 = pEntity3 = pEntity4 = nullptr;

	bSpawnedDrawer = false;

	if (uProgressBar)
	{
		uProgressBar = nullptr;
	}
	if (uProgressBG)
	{
		uProgressBG = nullptr;
	}

	debugTokens.clear();
}
} // namespace

TestWorld::TestWorld() : World("Test")
{
}

void TestWorld::OnActivated()
{
	pTestWorld = this;
	StartTests();
	BindInput(std::bind(&TestWorld::OnInput, this, _1));
	BindInput(&Test_OnInput);
}

void TestWorld::Tick(Time dt)
{
	World::Tick(dt);

	TestTick(dt);
}

void TestWorld::OnDeactivating()
{
	Cleanup();
}

bool TestWorld::OnInput(const EngineInput::Frame& frame)
{
	if (frame.IsReleased(GameInputType::Back))
	{
		Services::Game()->UI()->PushContext<OptionsUI>("Options");
		return true;
	}
	return false;
}

} // namespace LittleEngine
