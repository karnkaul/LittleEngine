#include "stdafx.h"
#include "TestLevel.h"
#include "LittleEngine.h"

namespace LittleEngine {
	using FileRW = GameUtils::FileRW;

	// Tests
	namespace _TestLevel {
		Level* pLevel;
		int actor0ID, playerID;
		bool bParented = false;
		bool bSoundPlayed = false, bMusicPlayed = false, bMusicStopped = false;
		
		void OnXPressed() {
			auto actor0 = pLevel->FindActor(actor0ID);
			auto player = pLevel->FindActor(playerID);
			if (actor0 != nullptr && player != nullptr) {
				if (!bParented) {
					actor0->GetTransform().SetParent(player->GetTransform());
					bParented = true;
				}
				else {
					actor0->GetTransform().UnsetParent();
					bParented = false;
				}
			}
		}
		void OnYPressed() {
			auto actor0 = pLevel->FindActor(actor0ID);
			auto player = pLevel->FindActor(playerID);
			if (actor0 != nullptr && player != nullptr) {
				if (!bParented) {
					actor0->GetTransform().SetParent(player->GetTransform(), false);
					bParented = true;
				}
				else {
					actor0->GetTransform().UnsetParent(false);
					bParented = false;
				}
			}
		}

		//Actor::wPtr _actor2, _actor3;
		int _actor2ID = 0, _actor3ID = 0;
		void OnEnterPressed() {
			Actor* pActor2 = pLevel->FindActor(_actor2ID);
			Actor* pActor3 = pLevel->FindActor(_actor3ID);
			/*auto actor2 = _actor2.lock();
			auto actor3 = _actor3.lock();*/
			if (pActor2 == nullptr && pActor3 == nullptr) {
				pActor2 = pLevel->SpawnActor<Actor>("Yellow Circle", true, Vector2(-300, 300));
				if (pActor2) {
					_actor2ID = pActor2->GetActorID();
					auto rc0 = pActor2->AddComponent<RenderComponent>();
					rc0->SetCircleRenderable(ShapeData(Vector2(100, 0), Colour::Yellow));
					auto t0 = pActor2->AddCollider<CircleCollider>();
					t0->SetCircle(100);
				}
				
				pActor3 = pLevel->SpawnActor<Actor>("Blue Rectangle", true);
				if (pActor3) {
					_actor3ID = pActor3->GetActorID();
					auto rc1 = pActor3->AddComponent<RenderComponent>();
					rc1->SetRectangleRenderable(ShapeData(Vector2(600, 100), Colour::Blue));
					auto t1 = pActor3->AddCollider<AABBCollider>();
					t1->SetBounds(AABBData(300, 50));
				}
			}
			else {
				pActor2->Destruct();
				pActor3->Destruct();
			}
		}

		Actor* _pTextActor = nullptr;
		Actor* _pCloneTest = nullptr;
		ParticleSystem* _pParticlesTest = nullptr;
		bool bContinuous = true;
		void OnSelectPressed() {
			if (_pParticlesTest->IsPlaying()) _pParticlesTest->Stop();
			else {
				_pParticlesTest->ToggleActive(true);
				_pParticlesTest->SetNormalisedPosition(GameUtils::Vector2(Maths::Random::Range(-Fixed::One, Fixed::One), Maths::Random::Range(-Fixed::One, Fixed::One)));
				_pParticlesTest->Start();
			}
		}

		UIButtonDrawer* pButtonDrawer = nullptr;
		Fixed elapsed;
		bool bModal = true;
		bool bSpawnedDrawer = false;
		std::vector<GameUtils::Delegate<>::Token> debugTokens;

		bool bToSpawnDialogue = false;
		bool bSpawnedDialog = false;
		UIDialogue* pDialogue = nullptr;

		void SpawnDialogue() {
			pDialogue = pLevel->GetUIController().SpawnContext<UIDialogue>();
			UIDialogueData data;
			data.bDestroyOnReturn = true;
			data.titleUIText = UIText("Title", 25, Colour::Black);
			data.contentUIText = UIText("Content goes here", 15, Colour::Black);
			data.headerBG = Colour::Cyan;
			data.contentBG = Colour::White;
			data.buttonData = UIButtonData::DebugButton();
			debugTokens.push_back(pDialogue->InitDialogue(std::move(data), "OK", []() { Logger::Log("OK pressed!"); }));
			debugTokens.push_back(pDialogue->AddOtherButton("Cancel", []() { pDialogue->Destruct(); }, false));
			pDialogue->SetActive(true);
			bToSpawnDialogue = false;
		}
		
		void UpdateTests(const Fixed& deltaMS) {
			elapsed += deltaMS;
			if (elapsed >= 1000 && !bSpawnedDrawer) {
				bSpawnedDrawer = true;
				pButtonDrawer = pLevel->GetUIController().SpawnContext<UIButtonDrawer>();
				pButtonDrawer->InitButtonDrawer(UIButtonDrawerData::DebugButtonDrawer(bModal));
				debugTokens.push_back(pButtonDrawer->AddButton("Button 0", []() { Logger::Log("Button 0 pressed!"); }));
				debugTokens.push_back(pButtonDrawer->AddButton("Button 1", []() { Logger::Log("Button 1 pressed!"); }));
				debugTokens.push_back(pButtonDrawer->AddButton("Button 2", []() { Logger::Log("Button 2 pressed!"); }));
				debugTokens.push_back(pButtonDrawer->AddButton("Button 3", []() { Logger::Log("Button 3 pressed!"); }));
				debugTokens.push_back(pButtonDrawer->AddButton("Button 4", []() { Logger::Log("Button 4 pressed!"); }));
				debugTokens.push_back(pButtonDrawer->AddButton("Dialogue", []() { bToSpawnDialogue = true; }));
				if (bModal) debugTokens.push_back(pButtonDrawer->AddButton("Cancel", []() { pButtonDrawer->Destruct(); }));
				pButtonDrawer->SetActive(true);
			}

			if (bToSpawnDialogue) {
				SpawnDialogue();
			}
		}

		void RenderTests() {

		}

		void CleanupTests() {
			elapsed = 0;
			bSpawnedDrawer = false;
			if (auto actor2 = pLevel->FindActor(_actor2ID)) {
				actor2->Destruct();
			}
			if (auto actor3 = pLevel->FindActor(_actor3ID)) {
				actor3->Destruct();
			}
			if (_pTextActor) _pTextActor = nullptr;
			if (_pCloneTest) _pCloneTest->Destruct();
			_pCloneTest = nullptr;
			if (_pParticlesTest) _pParticlesTest->Destruct();
			_pParticlesTest = nullptr;
			debugTokens.clear();
		}

		Actor* p_Actor0 = nullptr;
		Actor* p_Actor1 = nullptr;
		Player* p_Player = nullptr;
		GData psGData;
		ParticleSystem* p_ParticleSystem = nullptr;
	}

	void TestLevel::LoadAndSpawn() {
		Logger::Log(*this, "Loading Assets, spawning prototypes...", Logger::Severity::Debug);
		m_pEngine->GetAssetManager().LoadAll(AssetManifestData("AssetManifests/TestLevel.amf").GetManifest());

		_TestLevel::p_Actor0 = SpawnPrototype<Actor>("Actor0-RectangleRenderer", Vector2(300, 200));
		if (_TestLevel::p_Actor0) {
			_TestLevel::actor0ID = _TestLevel::p_Actor0->GetActorID();
			auto rc0 = _TestLevel::p_Actor0->AddComponent<RenderComponent>();
			rc0->SetRectangleRenderable(ShapeData(Vector2(300, 100), Colour::Magenta));
		}

		if ((_TestLevel::p_Actor1 = SpawnPrototype<Actor>("Actor1-TextRenderer"))) {
			auto rc = _TestLevel::p_Actor1->AddComponent<RenderComponent>();
			auto& tr = rc->SetTextRenderable("Hello World!");
			tr.m_layer = LayerID::UI;
			tr.SetColour(Colour(200, 150, 50)).SetSize(50);
		}

		_TestLevel::p_Player = SpawnPrototype<Player>("Player");
		if (_TestLevel::p_Player) {
			TextureAsset* texture = GetAssetManager().Load<TextureAsset>("Ship.png");
			_TestLevel::p_Player->InitPlayer(*this, *texture, AABBData(40, 40));
		}

		FileRW reader("Assets/VFX/Fire0/Fire0_noloop.psdata");
		_TestLevel::psGData.Marshall(reader.ReadAll(true));
		_TestLevel::p_ParticleSystem = SpawnPrototype<ParticleSystem>("ExplodePS");
	}

	void TestLevel::Tick(Fixed deltaMS) {
		_TestLevel::UpdateTests(deltaMS);
		Level::Tick(deltaMS);
	}

	void TestLevel::OnActivated() {
		Logger::Log(*this, "Running Level", Logger::Severity::Debug);

		if (_TestLevel::p_Actor0) {
			Actor* actor0 = CloneActor<Actor>(*_TestLevel::p_Actor0);
			actor0->GetTransform().localPosition = { 300, 200 };
			_TestLevel::actor0ID = actor0->GetActorID();
			auto rc0 = actor0->AddComponent<RenderComponent>();
			rc0->SetRectangleRenderable(ShapeData(Vector2(300, 100), Colour::Magenta));
			_TestLevel::_pTextActor = actor0;
		}
		
		if (_TestLevel::p_Actor1) {
			auto actor1 = CloneActor<Actor>(*_TestLevel::p_Actor1);
			actor1->GetTransform().localPosition = Graphics::NToWorld({ 0, Fixed(0.9f) });
			auto rc = actor1->AddComponent<RenderComponent>();
			auto& tr = rc->SetTextRenderable("Hello World!");
			tr.m_layer = LayerID::UI;
			tr.SetColour(Colour(200, 150, 50)).SetSize(50);
		}

		if (_TestLevel::p_Player) {
			Player* player = CloneActor<Player>(*_TestLevel::p_Player);
			player->GetTransform().localPosition = Vector2(-200, -300);
			_TestLevel::playerID = player->GetActorID();
		}

		RegisterScopedInput(GameInput::Return, std::bind(&TestLevel::OnQuitPressed, this), OnKey::Released);
		RegisterScopedInput(GameInput::X, &_TestLevel::OnXPressed, OnKey::Released);
		RegisterScopedInput(GameInput::Y, &_TestLevel::OnYPressed, OnKey::Released);
		RegisterScopedInput(GameInput::Enter, &_TestLevel::OnEnterPressed, OnKey::Released);
		RegisterScopedInput(GameInput::Select, &_TestLevel::OnSelectPressed, OnKey::Released);

		if (_TestLevel::p_ParticleSystem) {
			_TestLevel::_pParticlesTest = CloneActor<ParticleSystem>(*_TestLevel::p_ParticleSystem);
			ParticleSystemData psData(GetAssetManager(), _TestLevel::psGData);
			_TestLevel::_pParticlesTest->InitParticleSystem(std::move(psData));
		}

		// Tests
		_TestLevel::pLevel = this;
		_TestLevel::bSoundPlayed = _TestLevel::bMusicPlayed = false;

		if (auto pSpriteTest = SpawnActor<Actor>("Sprite Coords", true)) {
			pSpriteTest->GetTransform().localPosition = Vector2(-100, -100);
			RenderComponent* pRc = pSpriteTest->AddComponent<RenderComponent>();
			pRc->SetSpriteRenderable("Ship.png");
			SpriteRenderable* pR = dynamic_cast<SpriteRenderable*>(pRc->GetRenderable());
			pR->Crop(Vector2(10, 10), Vector2(50, 50));
		}
		
		GetAudioManager().PlayMusic("TestMusic.ogg", Fixed::OneHalf);
		/*Spawner::Init(*this);
		Spawner::VFXExplode_Warm();*/
	}

	void TestLevel::OnClearing() {
		_TestLevel::CleanupTests();
	}

	void RenderTests(Level*) {
		_TestLevel::RenderTests();
	}

	void TestLevel::PostRender() {
		if (m_clock.GetElapsedMilliSeconds() > 2000 && !_TestLevel::bSoundPlayed) {
			_TestLevel::bSoundPlayed = true;
			GetAudioManager().PlaySFX("TestSound.wav", Fixed(2, 10));
		}	
		/*if (clock.GetElapsedMilliSeconds() > 2100 && !_TestLevel::musicPlayed) {
			_TestLevel::musicPlayed = true;
			if (!GetAudioManager().PlaySFX("TestSound_b.wav", Fixed::Half)) {
				Logger::Log(*this, "Could not play SFX!", Logger::Severity::Error);
			}
			GetAudioManager().SetMusicVolume(Fixed::One);
			GetAudioManager().SwitchTrack("TestMusic_0.ogg", Fixed::Half);
		}
		if (clock.GetElapsedMilliSeconds() > 5000 && !_TestLevel::musicStopped) {
			_TestLevel::musicStopped = true;
			GetAudioManager().SwitchTrack("TestMusic.ogg", Fixed::Half, Fixed(3));
		}*/
		RenderTests(this);
	}

	void TestLevel::OnQuitPressed() {
		LoadLevel(m_pEngine->GetActiveLevelID() - 1);
	}
}
