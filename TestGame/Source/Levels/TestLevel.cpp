#include "stdafx.h"
#include "TestLevel.h"

namespace LittleEngine {
	// Tests
	namespace _TestLevel {
		Level* level;
		Actor::wPtr _actor0, _player;
		bool bParented = false;
		bool bSoundPlayed = false, bMusicPlayed = false, bMusicStopped = false;
		
		void OnXPressed() {
			auto actor0 = _actor0.lock();
			auto player = _player.lock();
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
			auto actor0 = _actor0.lock();
			auto player = _player.lock();
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

		Actor::wPtr _actor2, _actor3;
		void OnEnterPressed() {
			auto actor2 = _actor2.lock();
			auto actor3 = _actor3.lock();
			if (actor2 == nullptr && actor3 == nullptr) {
				_actor2 = level->SpawnActor<Actor>("Yellow Circle", Vector2(-300, 300));
				if (actor2 = _actor2.lock()) {
					auto rc0 = actor2->AddComponent<RenderComponent>();
					auto& r0 = rc0->SetCircleRenderer(ShapeData(Vector2(100, 0), Colour::Yellow));
					auto t0 = actor2->AddCollider<CircleCollider>();
					t0->SetCircle(100);
				}
				
				_actor3 = level->SpawnActor<Actor>("Blue Rectangle");
				if (actor3 = _actor3.lock()) {
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

		Actor::Ptr _textActor;
		Actor::wPtr _cloneTest;
		void OnSelectPressed() {
			Vector2 normalisedPosition = Vector2(Maths::Random::Range(Fixed(-1), Fixed(1)), Maths::Random::Range(Fixed(-1), Fixed(1)));
			Vector2 position = level->GetWorld().NormalisedToWorldPoint(normalisedPosition);
			auto v = Spawner::VFXExplode(position);
			v->Play();
			//if (_textActor) {
			//	if (!_cloneTest.lock()) {
			//		/*std::unique_ptr<Renderer> r = pTextRenderer->UClone();
			//		uTextRenderer = std::unique_ptr<TextRenderer>(dynamic_cast<TextRenderer*>(r.release()));*/
			//		_cloneTest = level->CloneActor<Actor>(*_textActor);
			//		auto clone = _cloneTest.lock();
			//		if (clone) {
			//			clone->GetTransform().localPosition = Vector2::Zero;
			//		}
			//	}
			//	else {
			//		_cloneTest.lock()->Destruct();
			//	}
			//}
		}

		void RenderTests(RenderParams& params) {
			
		}

		void CleanupTests() {
			if (auto actor2 = _actor2.lock()) {
				actor2->Destruct();
			}
			if (auto actor3 = _actor3.lock()) {
				actor3->Destruct();
			}
			if (_textActor) _textActor = nullptr;
			if (_cloneTest.lock()) _cloneTest.lock()->Destruct();
		}
	}

	void TestLevel::LoadAssets() {
		Logger::Log(*this, "Loading Assets...", Logger::Severity::Debug);
		engine->GetAssetManager().LoadAll(AssetManifestData("AssetManifests/TestLevel.amf").GetManifest());
	}

	void TestLevel::Activate() {
		Logger::Log(*this, "Running Level", Logger::Severity::Debug);

		_TestLevel::_actor0 = SpawnActor<Actor>("Actor0-RectangleRenderer", Vector2(300, 200));
		if (auto actor0 = _TestLevel::_actor0.lock()) {
			auto rc0 = actor0->AddComponent<RenderComponent>();
			rc0->SetRectangleRenderer(ShapeData(Vector2(300, 100), Colour::Magenta));
			_TestLevel::_textActor = actor0;
		}
		
		if (auto actor1 = SpawnActor<Actor>("Actor1-TextRenderer").lock()) {
			actor1->SetNormalisedPosition(Vector2(0, Fixed(0.9f)));
			auto rc = actor1->AddComponent<RenderComponent>();
			auto& tr = rc->SetTextRenderer("Hello World!");
			tr.layer = LayerID::UI;
			tr.SetColour(Colour(200, 150, 50)).SetSize(50);
			//_TestLevel::_textActor = actor1;
		}

		player = GetOrSpawnPlayer("Ship.png", AABBData(40, 40), Vector2(-200, -300));
		//_TestLevel::_textActor = player.lock();

		RegisterScopedInput(GameInput::Return, std::bind(&TestLevel::OnQuitPressed, this), OnKey::Released);
		RegisterScopedInput(GameInput::X, &_TestLevel::OnXPressed, OnKey::Released);
		RegisterScopedInput(GameInput::Y, &_TestLevel::OnYPressed, OnKey::Released);
		RegisterScopedInput(GameInput::Enter, &_TestLevel::OnEnterPressed, OnKey::Released);
		RegisterScopedInput(GameInput::Select, &_TestLevel::OnSelectPressed, OnKey::Released);

		// Tests
		_TestLevel::_player = this->player;
		_TestLevel::level = this;
		_TestLevel::bSoundPlayed = _TestLevel::bMusicPlayed = false;
		
		GetAudioManager().PlayMusic("TestMusic.ogg", Fixed::OneHalf);
		Spawner::Init(*this);
		Spawner::VFXExplode_Warm();
	}

	void TestLevel::OnClearing() {
		_TestLevel::CleanupTests();
	}

	void RenderTests(Level* level, std::vector<Actor::Ptr>& actors, RenderParams& params) {
		if (!actors.empty()) {
			actors[0]->GetTransform().Rotate(2);
		}
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
		RenderTests(this, actors, _params);
	}

	void TestLevel::OnQuitPressed() {
		LoadLevel(engine->GetActiveLevelID() - 1);
	}
}
