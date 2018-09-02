#include "stdafx.h"
#include "TestLevel.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Engine/Logger/Logger.h"
#include "Entities/Actor.h"
#include "Components/RenderComponent.h"
#include "Components/ControllerComponent.h"
#include "SFMLInterface/Input.h"
#include "SFMLInterface/Rendering/RenderParams.h"
#include "SFMLInterface/Rendering/RenderFactory.h"
#include "SFMLInterface/Rendering/TextRenderer.h"
#include "SFMLInterface/Rendering/ShapeRenderer.h"
#include "SFMLInterface/Assets.h"
#include "Engine/Physics/Collider.h"

namespace LittleEngine {
	// Tests
	namespace _TestLevel {
		Action::Token token0, token1;
		Level* level;
		Actor::wPtr _actor0, _actor1;
		bool parented = false;
		
		void OnXPressed() {
			auto actor0 = _actor0.lock();
			auto actor1 = _actor1.lock();
			if (actor0 != nullptr && actor1 != nullptr) {
				if (!parented) {
					actor0->GetTransform().SetParent(actor1->GetTransform());
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
			auto actor1 = _actor1.lock();
			if (actor0 != nullptr && actor1 != nullptr) {
				if (!parented) {
					actor0->GetTransform().SetParent(actor1->GetTransform(), false);
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
				_actor2 = level->SpawnActor("Yellow Circle");
				auto actor2 = _actor2.lock();
				if (actor2 != nullptr) {
					actor2->GetTransform().localPosition = Vector2(-300, 300);
					auto rc0 = actor2->AddComponent<RenderComponent>();
					auto& r0 = rc0->SetCircleRenderer(ShapeData(Vector2(100, 0), Colour::Yellow));
					auto t0 = actor2->AddCollider<CircleCollider>();
					t0->SetCircle(100);
				}
				
				_actor3 = level->SpawnActor("Blue Rectangle");
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
			if (auto actor1 = _actor1.lock()) {
				std::shared_ptr<AABBCollider> ac = actor1->GetCollider<AABBCollider>();
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

		void CleanupTests() {
			token0 = token1 = token2 = token3 = nullptr;
			if (auto actor1 = _actor1.lock()) {
				actor1->Destruct();
			}
			if (auto actor2 = _actor2.lock()) {
				actor2->Destruct();
			}
			if (auto actor3 = _actor3.lock()) {
				actor3->Destruct();
			}
		}
	}

	TestLevel::TestLevel(Engine& engine) : Level("TestLevel", engine) {
		Logger::Log(*this, "Running Level", Logger::Severity::Debug);

		auto actor0 = SpawnActor("Actor0-RectangleRenderer").lock();
		if (actor0 != nullptr) {
			actor0->GetTransform().localPosition = Vector2(300, 200);
			auto rc0 = actor0->AddComponent<RenderComponent>();
			rc0->SetRectangleRenderer(ShapeData(Vector2(300, 100), Colour::Magenta));
		}

		auto player = SpawnActor("Player").lock();
		if (player != nullptr) {
			player->GetTransform().localPosition = Vector2(-200, -300);
			player->AddComponent<ControllerComponent>();
			auto playerRenderer = player->AddComponent<RenderComponent>();
			playerRenderer->SetSpriteRenderer("Assets/Ship.png");
			playerRenderer->SetLayer(LayerID::Player);
			auto collider = player->AddCollider<AABBCollider>();
			collider->SetBounds(AABBData(40, 40));
		}

		auto actor1 = SpawnActor("Actor1-TextRenderer").lock();
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
		_TestLevel::_actor1 = player;
		_TestLevel::token0 = GetInputHandler().Register(GameInput::X, &_TestLevel::OnXPressed, OnKey::Released);
		_TestLevel::token1 = GetInputHandler().Register(GameInput::Y, &_TestLevel::OnYPressed, OnKey::Released);
		_TestLevel::level = this;
		_TestLevel::token2 = GetInputHandler().Register(GameInput::Enter, &_TestLevel::OnEnterPressed, OnKey::Released);
		_TestLevel::token3 = GetInputHandler().Register(GameInput::RB, &_TestLevel::OnLBPressed, OnKey::Released);
	}

	void RenderTests(Level* level, std::vector<Actor::Ptr>& actors, RenderParams& params) {
		if (!actors.empty()) {
			actors[0]->GetTransform().Rotate(2);
		}
	}

	void TestLevel::Render(RenderParams & params) {
		RenderTests(this, actors, params);
 		Level::Render(params);
	}

	void TestLevel::OnQuitPressed() {
		_TestLevel::CleanupTests();
		engine->LoadLevel(LevelID::BootLevel);
	}
}
