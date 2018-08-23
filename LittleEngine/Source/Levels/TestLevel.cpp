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

namespace Game {
	// Tests
	namespace _TestLevel {
		Action::Token token0, token1, token2;
		Actor* actor0 = nullptr, *actor1 = nullptr;
		Level* level;
		std::shared_ptr<Actor> actor2 = nullptr, actor3 = nullptr;
		void OnXPressed() {
			if (actor0 != nullptr && actor1 != nullptr) {
				actor0->GetTransform()->SetParent(actor1->GetTransform());
				token0 = token1 = nullptr;
			}
		}
		void OnYPressed() {
			if (actor0 != nullptr && actor1 != nullptr) {
				actor0->GetTransform()->SetParent(actor1->GetTransform(), false);
				token0 = token1 = nullptr;
			}
		}
		void OnEnterPressed() {
			if (actor2 == nullptr) {
				actor2 = level->NewActor("Yellow Shape");
				actor2->GetTransform()->localPosition = Vector2(-300, 300);
				auto rc0 = actor2->AddComponent<RenderComponent>();
				rc0->SetCircleRenderer(ShapeData(Vector2(200, 0), Colour::Yellow));
				auto t0 = actor2->AddCollider<AABBCollider>();
				t0->SetBounds(AABB(50, 50));
				
				actor3 = level->NewActor("Blue Rectangle");
				//actor3->GetTransform()->localPosition = Vector2(300, 100);
				auto rc1 = actor3->AddComponent<RenderComponent>();
				rc1->SetRectangleRenderer(ShapeData(Vector2(600, 100), Colour::Blue));
				auto t1 = actor3->AddCollider<AABBCollider>();
				t1->SetBounds(AABB(300, 50));
			}
			else { 
				level->DestroyActor(actor2);
				level->DestroyActor(actor3);
				actor2 = nullptr;
				actor3 = nullptr;
			}
		}
	}

	TestLevel::TestLevel(Engine& engine) : Level("TestLevel", engine) {
		Logger::Log(*this, "Running Level", Logger::Severity::Debug);

		auto actor0 = NewActor("Actor0-RectangleRenderer");
		actor0->GetTransform()->localPosition = Vector2(300, 200);
		auto rc0 = actor0->AddComponent<RenderComponent>();
		rc0->SetRectangleRenderer(ShapeData(Vector2(300, 100), Colour::Magenta));

		auto player = NewActor("Player");
		player->GetTransform()->localPosition = Vector2(-200, -300);
		player->AddComponent<ControllerComponent>();
		auto playerRenderer = player->AddComponent<RenderComponent>();
		playerRenderer->SetSpriteRenderer("Assets/Ship.png");
		auto collider = player->AddCollider<AABBCollider>();
		collider->SetBounds(AABB(64, 64));
		//actor0->GetTransform()->SetParent(pawn0->GetTransform());

		//pawn0 = actor0;		// Must not compile
		auto actor1 = NewActor("Actor1-TextRenderer");
		Fixed yPos = this->engine.GetWorld().GetScreenSize().y / 2;
		auto rc = actor1->AddComponent<RenderComponent>();
		auto& tr = rc->SetTextRenderer("Hello World!");
		tr.SetColour(Colour(200, 150, 50)).SetSize(50);
		yPos -= tr.GetBounds().y;
		actor1->GetTransform()->localPosition = Vector2(0, yPos - 50);

		// Tests
		_TestLevel::actor0 = actor0.get();
		_TestLevel::actor1 = player.get();
		_TestLevel::token0 = GetInputHandler().Register(GameInput::X, &_TestLevel::OnXPressed, OnKey::Released);
		_TestLevel::token1 = GetInputHandler().Register(GameInput::Y, &_TestLevel::OnYPressed, OnKey::Released, true);
		_TestLevel::level = this;
		_TestLevel::token2 = GetInputHandler().Register(GameInput::Enter, &_TestLevel::OnEnterPressed, OnKey::Released);
	}

	TestLevel::~TestLevel() {
		_TestLevel::actor2 = nullptr;
		_TestLevel::actor3 = nullptr;
	}

	void RenderTests(Level* level, std::vector<std::shared_ptr<Actor> >& actors, RenderParams& params) {
		if (!actors.empty()) {
			actors[0]->GetTransform()->Rotate(2);
		}
	}

	void TestLevel::Render(RenderParams & params) {
		RenderTests(this, actors, params);
 		Level::Render(params);
	}
}
