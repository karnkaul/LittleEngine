#include "stdafx.h"
#include "TestWorld.h"
#include "GameFramework/GameFramework.h"

namespace LittleEngine
{
namespace
{
bool bSpawnColliderMinefield = false;
EngineInput::Token t0;
TestWorld* pTestWorld = nullptr;

Entity *pEntity0 = nullptr, *pEntity1 = nullptr;
Entity *pEntity2 = nullptr, *pEntity3 = nullptr;

void OnEnter()
{
	if (!pEntity2 && !pEntity3)
	{
		pEntity2 = pTestWorld->Game()->NewEntity<Entity>("Yellow Circle", Vector2(-300, 300));
		if (pEntity2)
		{
			auto rc0 = pEntity2->AddComponent<RenderComponent>();
			rc0->m_pSFPrimitive->SetSize({100, 100}, SFShapeType::Circle)->SetPrimaryColour(Colour::Yellow);
			auto t0 = pEntity2->AddComponent<CollisionComponent>();
			t0->AddCircle(100);
		}

		pEntity3 = pTestWorld->Game()->NewEntity<Entity>("Blue Rectangle", Vector2(500, -200));
		if (pEntity3)
		{
			auto rc1 = pEntity3->AddComponent<RenderComponent>();
			rc1->m_pSFPrimitive->SetSize({600, 100}, SFShapeType::Rectangle)->SetPrimaryColour(Colour::Blue);
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
}

bool bLoopingPS = true;
ParticleSystem* pParticleSystem0 = nullptr;
void OnSelect()
{
	if (!pParticleSystem0)
		return;

	if (pParticleSystem0->IsPlaying())
		pParticleSystem0->Stop();
	else
	{
		//pParticleSystem0->SetEnabled(true);
		Fixed x = Maths::Random::Range(-Fixed::One, Fixed::One);
		Fixed y = Maths::Random::Range(-Fixed::One, Fixed::One);
		Vector2 worldPos = GFX::Project({x, y}, false);
		pParticleSystem0->m_transform.localPosition = worldPos;
		pParticleSystem0->Start();
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

bool OnInput(const EngineInput::Frame& frame)
{
	if (frame.IsReleased(GameInputType::Back) && pTestWorld)
	{
		if (pTestWorld->LoadWorld(0))
		{
		}
	}

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
			Entity* pE = pTestWorld->Game()->NewEntity<Entity>(name, Vector2(x, y));
			CollisionComponent* pCC = pE->AddComponent<CollisionComponent>();
			//pRC->m_pSFPrimitive->SetSize({600, 100}, SFShapeType::Rectangle)->SetPrimaryColour(Colour::Blue);
			pCC->AddAABB(AABBData({100, 100}));
		}
	}
}

void StartTests()
{
	pEntity0 = pTestWorld->Game()->NewEntity<Entity>("Entity0", {300, 200});
	auto rc0 = pEntity0->AddComponent<RenderComponent>();
	rc0->m_pSFPrimitive->SetSize({300, 100}, SFShapeType::Rectangle)->SetPrimaryColour(Colour::Magenta)->SetEnabled(true);

	pEntity1 = pTestWorld->Game()->NewEntity<Entity>("Entity1", GFX::Project({0, Fixed(0.9f)}, false));
	auto rc1 = pEntity1->AddComponent<RenderComponent>();
	FontAsset* font = pTestWorld->Game()->Repository()->GetDefaultFont();
	rc1->m_pSFPrimitive->SetText("Hello World!")
		->SetFont(*font)
		->SetTextSize(50)
		->SetPrimaryColour(Colour(200, 150, 50))
		->SetEnabled(true);

	pPlayer = pTestWorld->Game()->NewEntity<Player>("Player0", {0, -300});
	TextureAsset* pTexture = pTestWorld->Game()->Repository()->Load<TextureAsset>("Ship.png");
	PlayerData data(*pTexture, {PlayerCollider(AABBData({120, 60}), {0, -15}), PlayerCollider(AABBData({60, 80}))});
	pPlayer->InitPlayer(data);

	String path = bLoopingPS ? "Assets/VFX/Fire0/Fire0_loop.psdata" : "Assets/VFX/Fire0/Fire0_noloop.psdata";
	FileRW reader(path);
	GData psGData(reader.ReadAll(true));
	ParticleSystemData psData(psGData);
	pParticleSystem0 = pTestWorld->Game()->NewEntity<ParticleSystem>("Fire0");
	pParticleSystem0->InitParticleSystem(std::move(psData));
	pParticleSystem0->m_transform.localScale = {Fixed(2.0f), Fixed(2.0f)};
	pParticleSystem0->Stop();

	if (bSpawnColliderMinefield)
		SpawnColliderMinefield();
}

UIButtonDrawer* pButtonDrawer = nullptr;
bool bModal = true;
bool bSpawnedDrawer = false;
Vector<EngineInput::Token> debugTokens;

bool bToSpawnDialogue = false;
UIDialogue* pDialogue = nullptr;

//Fixed progress;
UPtr<UIProgressBar> uProgressBar = nullptr;
UPtr<UIElement> uProgressBG = nullptr;
Time elapsed = Time::Zero;

void SpawnDialogue()
{
	LayerID dialogueLayer = static_cast<LayerID>(LAYER_UI + 2);
	pDialogue = pTestWorld->Game()->UI()->PushContext<UIDialogue>(dialogueLayer);
	pDialogue->SetHeader(UIText("Title", 25, Colour::Black));
	pDialogue->SetContent(UIText("Content goes here", 15, Colour::Black), &Colour::White);
	debugTokens.push_back(pDialogue->AddMainButton("OK", []() { LOG_D("OK pressed!"); }, false));
	debugTokens.push_back(pDialogue->AddOtherButton("Cancel", []() { pDialogue->Destruct(); }, false));
	pDialogue->SetActive(true);
	bToSpawnDialogue = false;
}

void SpawnToggle()
{
	Fixed x = 300;
	Fixed y = 200;
	LayerID toggleLayer = static_cast<LayerID>(LAYER_UI + 4);
	UIContext* pParent = pTestWorld->Game()->UI()->PushContext<UIContext>(toggleLayer);
	pParent->GetRootElement()->m_transform.size = {x, y};
	UIWidgetStyle toggleStyle = UIWidgetStyle::GetDefault();
	toggleStyle.widgetSize = {x, y * Fixed::OneHalf};
	toggleStyle.background = Colour::Yellow;
	UIToggle* pToggle0 = pParent->AddWidget<UIToggle>("Toggle0", &toggleStyle);
	UIToggle* pToggle1 = pParent->AddWidget<UIToggle>("Toggle1", &toggleStyle);
	
	debugTokens.push_back(pToggle0->AddCallback([](bool bVal) { LOG_W("Toggle0 changed! %d", bVal); }));
	pToggle0->GetRoot()->m_transform.bAutoPad = true;
	pToggle0->SetText("Toggle 0")->SetOn(false)->GetRoot()->m_transform.nPosition = {0, 1};
	
	debugTokens.push_back(pToggle1->AddCallback([](bool bValue) { LOG_W("Toggle1 changed! %d", bValue); }));
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
		pButtonDrawer = pTestWorld->Game()->UI()->PushContext<UIButtonDrawer>();
		pButtonDrawer->m_bAutoDestroyOnCancel = !bModal;
		UIStyle panelStyle;
		panelStyle.size = {500, 600};
		panelStyle.fill = Colour(100, 100, 100, 100);
		pButtonDrawer->SetPanel(panelStyle);
		debugTokens.push_back(
			pButtonDrawer->AddButton("Button 0", []() { LOG_D("Button 0 pressed!"); }));
		debugTokens.push_back(
			pButtonDrawer->AddButton("Button 1", []() { LOG_D("Button 1 pressed!"); }));
		debugTokens.push_back(
			pButtonDrawer->AddButton("Button 2", []() { LOG_D("Button 2 pressed!"); }));
		debugTokens.push_back(
			pButtonDrawer->AddButton("Button 3", []() { LOG_D("Button 3 pressed!"); }));
		debugTokens.push_back(
			pButtonDrawer->AddButton("Toggle", &SpawnToggle));
		debugTokens.push_back(pButtonDrawer->AddButton("Dialogue", []() { bToSpawnDialogue = true; }));
		if (bModal)
			debugTokens.push_back(
				pButtonDrawer->AddButton("Cancel", []() { pButtonDrawer->Destruct(); }));
		pButtonDrawer->SetActive(true);
	}

	if (bToSpawnDialogue)
	{
		SpawnDialogue();
	}

	/*if (elapsed.AsSeconds() >= 2 && progress < Fixed(1.5f))
	{
		progress += Fixed(dt.AsSeconds());
		if (!uProgressBar)
		{
			uProgressBG = MakeUnique<UIElement>("Test ProgressBar BG");
			uProgressBG->m_transform.size = {500, 50};
			uProgressBG->SetPanel(Colour(255, 200, 100, 100));
			uProgressBar = MakeUnique<UIProgressBar>("Test ProgressBar");
			uProgressBar->InitProgressBar({500, 10}, Colour::Magenta);
			uProgressBar->m_transform.SetParent(uProgressBG->m_transform);
		}
		uProgressBar->SetProgress(progress);
		uProgressBar->Tick(dt);
		uProgressBG->Tick(dt);
	}*/
}

void Cleanup()
{
	elapsed = Time::Zero;
	pTestWorld = nullptr;
	
	pParticleSystem0 = nullptr;
	pPlayer = nullptr;

	pEntity0 = pEntity1 = pEntity2 = pEntity3 = nullptr;

	bSpawnedDrawer = false;

	if (uProgressBar)
		uProgressBar = nullptr;
	if (uProgressBG)
		uProgressBG = nullptr;
	debugTokens.clear();
}
} // namespace

TestWorld::TestWorld() : World("Test")
{
}

void TestWorld::OnActivated()
{
	pTestWorld = this;
	BindInput(&OnInput);
	StartTests();
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
} // namespace LittleEngine
