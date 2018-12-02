#include "stdafx.h"
#include "TestLevel.h"
#include "Entities/ParticleSystem.h"
#include "Utils.h"
#include "FileRW.h"
#include "GData.h"

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
					auto& r0 = rc0->SetCircleRenderable(ShapeData(Vector2(100, 0), Colour::Yellow));
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

		void UpdateTests(const Fixed& deltaTime) {
			
		}

		void RenderTests(RenderParams& params) {
			
		}

		void CleanupTests() {
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
		}
	}

	void TestLevel::LoadAssets() {
		Logger::Log(*this, "Loading Assets...", Logger::Severity::Debug);
		engine->GetAssetManager().LoadAll(AssetManifestData("AssetManifests/TestLevel.amf").GetManifest());
	}

	void TestLevel::Tick(Fixed deltaTime) {
		_TestLevel::UpdateTests(deltaTime);
		Level::Tick(deltaTime);
	}

	void TestLevel::OnActivated() {
		Logger::Log(*this, "Running Level", Logger::Severity::Debug);

		Actor* actor0 = SpawnActor<Actor>("Actor0-RectangleRenderer", true, Vector2(300, 200));
		if (actor0) {
			_TestLevel::actor0ID = actor0->GetActorID();
			auto rc0 = actor0->AddComponent<RenderComponent>();
			rc0->SetRectangleRenderable(ShapeData(Vector2(300, 100), Colour::Magenta));
			_TestLevel::_pTextActor = actor0;
		}
		
		if (auto actor1 = SpawnActor<Actor>("Actor1-TextRenderer", true)) {
			actor1->SetNormalisedPosition(Vector2(0, Fixed(0.9f)));
			auto rc = actor1->AddComponent<RenderComponent>();
			auto& tr = rc->SetTextRenderable("Hello World!");
			tr.layer = LayerID::UI;
			tr.SetColour(Colour(200, 150, 50)).SetSize(50);
			//_TestLevel::_textActor = actor1;
		}

		Player* player = SpawnActor<Player>("Player", true);
		if (player) {
			TextureAsset* texture = GetAssetManager().Load<TextureAsset>("Ship.png");
			player->InitPlayer(*this, *texture, AABBData(40, 40));
			player->GetTransform().localPosition = Vector2(-200, -300);
		}
		_TestLevel::_pTextActor = player;

		RegisterScopedInput(GameInput::Return, std::bind(&TestLevel::OnQuitPressed, this), OnKey::Released);
		RegisterScopedInput(GameInput::X, &_TestLevel::OnXPressed, OnKey::Released);
		RegisterScopedInput(GameInput::Y, &_TestLevel::OnYPressed, OnKey::Released);
		RegisterScopedInput(GameInput::Enter, &_TestLevel::OnEnterPressed, OnKey::Released);
		RegisterScopedInput(GameInput::Select, &_TestLevel::OnSelectPressed, OnKey::Released);

		FileRW reader("Assets/VFX/Fire0/Fire0_noloop.psdata");
		GData psGData(reader.ReadAll(true));
		ParticleSystemData psData(*this, psGData);

		_TestLevel::_pParticlesTest = SpawnActor<ParticleSystem>("ExplodePS", false);
		_TestLevel::_pParticlesTest->InitParticleSystem(std::move(psData));

		// Tests
		_TestLevel::playerID = player->GetActorID();
		_TestLevel::pLevel = this;
		_TestLevel::bSoundPlayed = _TestLevel::bMusicPlayed = false;
		
		GetAudioManager().PlayMusic("TestMusic.ogg", Fixed::OneHalf);
		/*Spawner::Init(*this);
		Spawner::VFXExplode_Warm();*/
	}

	void TestLevel::OnClearing() {
		_TestLevel::CleanupTests();
	}

	void RenderTests(Level* level, RenderParams& params) {
		_TestLevel::RenderTests(params);
	}

	void TestLevel::PostRender(const RenderParams& params) {
		if (clock.GetElapsedMilliSeconds() > 2000 && !_TestLevel::bSoundPlayed) {
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
		RenderParams _params(params);
		RenderTests(this, _params);
	}

	void TestLevel::OnQuitPressed() {
		LoadLevel(engine->GetActiveLevelID() - 1);
	}
}
