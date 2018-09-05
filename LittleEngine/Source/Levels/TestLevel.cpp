#include "stdafx.h"
#include "TestLevel.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Engine/Logger/Logger.h"
#include "Entities/Actor.h"
#include "Entities/VFX.h"
#include "Components/RenderComponent.h"
#include "Components/ControllerComponent.h"
#include "SFMLInterface/Input.h"
#include "SFMLInterface/Rendering/RenderParams.h"
#include "SFMLInterface/Rendering/RenderFactory.h"
#include "SFMLInterface/Rendering/TextRenderer.h"
#include "SFMLInterface/Rendering/ShapeRenderer.h"
#include "SFMLInterface/Assets.h"
#include "Engine/Physics/Collider.h"
#include "SFMLInterface/Assets.h"
#include "Components/SpriteAnimator.h"
#include "SFMLInterface/Rendering/SpriteRenderer.h"
#include "SFMLInterface/Rendering/TextRenderer.h"

namespace LittleEngine {
	// Tests
	namespace _TestLevel {
		Action::Token token0, token1;
		Level* level;
		Actor::wPtr _actor0, _player;
		bool parented = false;
		bool soundPlayed = false, musicPlayed = false, musicStopped = false;
		std::unique_ptr<SpriteRenderer> spriteRenderer = nullptr;
		std::unique_ptr<TextRenderer> textRenderer = nullptr;

		void OnXPressed() {
			auto actor0 = _actor0.lock();
			auto player = _player.lock();
			if (actor0 != nullptr && player != nullptr) {
				if (!parented) {
					actor0->GetTransform().SetParent(player->GetTransform());
					parented = true;
				}
				else {
					actor0->GetTransform().UnsetParent();
					parented = false;
				}
			}
		}
		void OnYPressed() {
			auto actor0 = _actor0.lock();
			auto player = _player.lock();
			if (actor0 != nullptr && player != nullptr) {
				if (!parented) {
					actor0->GetTransform().SetParent(player->GetTransform(), false);
					parented = true;
				}
				else {
					actor0->GetTransform().UnsetParent(false);
					parented = false;
				}
			}
		}

		Action::Token token2;
		Actor::wPtr _actor2, _actor3;
		void OnEnterPressed() {
			auto actor2 = _actor2.lock();
			auto actor3 = _actor3.lock();
			if (actor2 == nullptr && actor3 == nullptr) {
				_actor2 = level->SpawnActor<Actor>("Yellow Circle", Vector2(-300, 300));
				auto actor2 = _actor2.lock();
				if (actor2 != nullptr) {
					auto rc0 = actor2->AddComponent<RenderComponent>();
					auto& r0 = rc0->SetCircleRenderer(ShapeData(Vector2(100, 0), Colour::Yellow));
					auto t0 = actor2->AddCollider<CircleCollider>();
					t0->SetCircle(100);
				}
				
				_actor3 = level->SpawnActor<Actor>("Blue Rectangle");
				auto actor3 = _actor3.lock();
				if (actor3 != nullptr) {
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

		bool drawn = false;
		Action::Token token3;
		void OnLBPressed() {
			drawn = !drawn;
			if (auto player = _player.lock()) {
				std::shared_ptr<AABBCollider> ac = player->GetCollider<AABBCollider>();
				if (ac != nullptr) {
					ac->DrawDebugShape(drawn);
				}
			}
			if (auto actor2 = _actor2.lock()) {
				if (auto actor3 = _actor3.lock()) {
					std::shared_ptr<CircleCollider> cc = actor2->GetCollider<CircleCollider>();
					if (cc != nullptr) {
						cc->DrawDebugShape(drawn);
					}
					std::shared_ptr<AABBCollider> ac = actor3->GetCollider<AABBCollider>();
					if (ac != nullptr) {
						ac->DrawDebugShape(drawn);
					}
				}
			}
		}

		Action::Token token4;
		void OnSelectPressed() {
			std::weak_ptr<VFX> v = level->SpawnActor<VFX>("VFX");
			if (auto vfx = v.lock()) {
				Vector2 pos = Vector2(Maths::Random::Range(Fixed(-1), Fixed(1)), Maths::Random::Range(Fixed(-1), Fixed(1)));
				vfx->SetNormalisedPosition(pos);
				AssetPaths spriteSheet("VFX/Explode", 14, "", ".png");
				AssetPaths sfx("VFX/Explode", 1, "", ".wav");
				vfx->Init(spriteSheet, sfx, Fixed(1000), Fixed(3, 10));
			}
		}

		void CleanupTests() {
			token0 = token1 = token2 = token3 = nullptr;
			if (auto actor2 = _actor2.lock()) {
				actor2->Destruct();
			}
			if (auto actor3 = _actor3.lock()) {
				actor3->Destruct();
			}
			spriteRenderer = nullptr;
			textRenderer = nullptr;
		}
	}

	TestLevel::TestLevel(Engine& engine) : Level("TestLevel", engine) {
		Logger::Log(*this, "Running Level", Logger::Severity::Debug);

		auto actor0 = SpawnActor<Actor>("Actor0-RectangleRenderer", Vector2(300, 200)).lock();
		if (actor0 != nullptr) {
			auto rc0 = actor0->AddComponent<RenderComponent>();
			rc0->SetRectangleRenderer(ShapeData(Vector2(300, 100), Colour::Magenta));
		}

		GetOrSpawnPlayer("Ship.png", AABBData(40, 40), Vector2(-200, -300));
		
		auto actor1 = SpawnActor<Actor>("Actor1-TextRenderer").lock();
		if (actor1 != nullptr) {
			actor1->SetNormalisedPosition(Vector2(0, Fixed(0.9f)));
			auto rc = actor1->AddComponent<RenderComponent>();
			auto& tr = rc->SetTextRenderer("Hello World!");
			tr.layer = LayerID::UI;
			tr.SetColour(Colour(200, 150, 50)).SetSize(50);
		}

		quitLevelToken = GetInputHandler().Register(GameInput::Return, std::bind(&TestLevel::OnQuitPressed, this), OnKey::Released);
		// Tests
		_TestLevel::drawn = false;
		_TestLevel::_actor0 = actor0;
		_TestLevel::_player = player;
		_TestLevel::token0 = GetInputHandler().Register(GameInput::X, &_TestLevel::OnXPressed, OnKey::Released);
		_TestLevel::token1 = GetInputHandler().Register(GameInput::Y, &_TestLevel::OnYPressed, OnKey::Released);
		_TestLevel::level = this;
		_TestLevel::token2 = GetInputHandler().Register(GameInput::Enter, &_TestLevel::OnEnterPressed, OnKey::Released);
		_TestLevel::token3 = GetInputHandler().Register(GameInput::RB, &_TestLevel::OnLBPressed, OnKey::Released);
		_TestLevel::soundPlayed = _TestLevel::musicPlayed = false;
		_TestLevel::token4 = GetInputHandler().Register(GameInput::Select, &_TestLevel::OnSelectPressed, OnKey::Released);
		
		GetAudioManager().PlayMusic("TestMusic.ogg", Fixed::Half);
	}

	void RenderTests(Level* level, std::vector<Actor::Ptr>& actors, RenderParams& params) {
		if (!actors.empty()) {
			actors[0]->GetTransform().Rotate(2);
		}
	}

	void TestLevel::Render(RenderParams & params) {
		if (clock.GetElapsedMilliSeconds() > 2000 && !_TestLevel::soundPlayed) {
			_TestLevel::soundPlayed = true;
			GetAudioManager().PlaySFX("TestSound.wav", Fixed(2, 10));
		}	
		if (clock.GetElapsedMilliSeconds() > 2100 && !_TestLevel::musicPlayed) {
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
		}
		if (_TestLevel::spriteRenderer != nullptr) {
			params.screenPosition = GetWorld().WorldToScreenPoint(Vector2(600, -200));
			_TestLevel::spriteRenderer->Render(params);
		}
		if (_TestLevel::textRenderer != nullptr) {
			params.screenPosition = GetWorld().WorldToScreenPoint(Vector2(200, -100));
			_TestLevel::textRenderer->Render(params);
		}
		RenderTests(this, actors, params);
 		Level::Render(params);
	}

	void TestLevel::OnQuitPressed() {
		_TestLevel::CleanupTests();
		engine->LoadLevel(LevelID::BootLevel);
	}
}
