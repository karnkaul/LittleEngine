#include "stdafx.h"
#include "TestLevel.h"

namespace LittleEngine {
	// Tests
	namespace _TestLevel {
		Level* level;
		int actor0ID, playerID;
		bool bParented = false;
		bool bSoundPlayed = false, bMusicPlayed = false, bMusicStopped = false;
		
		void OnXPressed() {
			auto actor0 = level->FindActor(actor0ID);
			auto player = level->FindActor(playerID);
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
			auto actor0 = level->FindActor(actor0ID);
			auto player = level->FindActor(playerID);
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
			Actor* actor2 = level->FindActor(_actor2ID);
			Actor* actor3 = level->FindActor(_actor3ID);
			/*auto actor2 = _actor2.lock();
			auto actor3 = _actor3.lock();*/
			if (actor2 == nullptr && actor3 == nullptr) {
				actor2 = level->SpawnActor<Actor>("Yellow Circle", true, Vector2(-300, 300));
				if (actor2) {
					_actor2ID = actor2->GetActorID();
					auto rc0 = actor2->AddComponent<RenderComponent>();
					auto& r0 = rc0->SetCircleRenderer(ShapeData(Vector2(100, 0), Colour::Yellow));
					auto t0 = actor2->AddCollider<CircleCollider>();
					t0->SetCircle(100);
				}
				
				actor3 = level->SpawnActor<Actor>("Blue Rectangle", true);
				if (actor3) {
					_actor3ID = actor3->GetActorID();
					auto rc1 = actor3->AddComponent<RenderComponent>();
					rc1->SetRectangleRenderer(ShapeData(Vector2(600, 100), Colour::Blue));
					auto t1 = actor3->AddCollider<AABBCollider>();
					t1->SetBounds(AABBData(300, 50));
				}
			}
			else {
				actor2->Destruct();
				actor3->Destruct();
			}
		}

		Actor* _textActor = nullptr;
		Actor* _cloneTest = nullptr;
		void OnSelectPressed() {
			Vector2 normalisedPosition = Vector2(Maths::Random::Range(Fixed(-1), Fixed(1)), Maths::Random::Range(Fixed(-1), Fixed(1)));
			Vector2 position = level->GetWorld().NormalisedToWorldPoint(normalisedPosition);
			auto v = Spawner::VFXExplode(position);
			v->Play();
			if (_textActor) {
				if (!_cloneTest) {
					_cloneTest = level->CloneActor<Actor>(*_textActor);
					if (_cloneTest) {
						_cloneTest->GetTransform().localPosition = Vector2::Zero;
					}
				}
				else {
					_cloneTest->Destruct();
					_cloneTest = nullptr;
				}
			}
		}

		void RenderTests(RenderParams& params) {
			
		}

		void CleanupTests() {
			if (auto actor2 = level->FindActor(_actor2ID)) {
				actor2->Destruct();
			}
			if (auto actor3 = level->FindActor(_actor3ID)) {
				actor3->Destruct();
			}
			if (_textActor) _textActor = nullptr;
			if (_cloneTest) _cloneTest->Destruct();
			_cloneTest = nullptr;
		}
	}

	void TestLevel::LoadAssets() {
		Logger::Log(*this, "Loading Assets...", Logger::Severity::Debug);
		engine->GetAssetManager().LoadAll(AssetManifestData("AssetManifests/TestLevel.amf").GetManifest());
	}

	void TestLevel::OnActivated() {
		Logger::Log(*this, "Running Level", Logger::Severity::Debug);

		Actor* actor0 = SpawnActor<Actor>("Actor0-RectangleRenderer", true, Vector2(300, 200));
		if (actor0) {
			_TestLevel::actor0ID = actor0->GetActorID();
			auto rc0 = actor0->AddComponent<RenderComponent>();
			rc0->SetRectangleRenderer(ShapeData(Vector2(300, 100), Colour::Magenta));
			_TestLevel::_textActor = actor0;
		}
		
		if (auto actor1 = SpawnActor<Actor>("Actor1-TextRenderer", true)) {
			actor1->SetNormalisedPosition(Vector2(0, Fixed(0.9f)));
			auto rc = actor1->AddComponent<RenderComponent>();
			auto& tr = rc->SetTextRenderer("Hello World!");
			tr.layer = LayerID::UI;
			tr.SetColour(Colour(200, 150, 50)).SetSize(50);
			//_TestLevel::_textActor = actor1;
		}

		Player* player = SpawnActor<Player>("", true);
		if (player) {
			TextureAsset* texture = GetAssetManager().Load<TextureAsset>("Ship.png");
			player->Init(*texture, AABBData(40, 40));
			player->GetTransform().localPosition = Vector2(-200, -300);
		}
		//Player& player = GetOrSpawnPlayer("Ship.png", AABBData(40, 40), Vector2(-200, -300));
		//Player* player = SpawnActor<Player>()
		_TestLevel::_textActor = player;

		RegisterScopedInput(GameInput::Return, std::bind(&TestLevel::OnQuitPressed, this), OnKey::Released);
		RegisterScopedInput(GameInput::X, &_TestLevel::OnXPressed, OnKey::Released);
		RegisterScopedInput(GameInput::Y, &_TestLevel::OnYPressed, OnKey::Released);
		RegisterScopedInput(GameInput::Enter, &_TestLevel::OnEnterPressed, OnKey::Released);
		RegisterScopedInput(GameInput::Select, &_TestLevel::OnSelectPressed, OnKey::Released);

		// Tests
		_TestLevel::playerID = player->GetActorID();
		_TestLevel::level = this;
		_TestLevel::bSoundPlayed = _TestLevel::bMusicPlayed = false;
		
		GetAudioManager().PlayMusic("TestMusic.ogg", Fixed::OneHalf);
		Spawner::Init(*this);
		Spawner::VFXExplode_Warm();
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
