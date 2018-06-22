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

namespace Game {
	// Tests
	namespace _TestLevel {
		bool parentSet = false;
		Action::Token token0, token1;
		Actor* actor0 = nullptr, *actor1 = nullptr;
		void OnXPressed() {
			if (!parentSet && actor0 != nullptr && actor1 != nullptr) {
				actor0->GetTransform()->SetParent(actor1->GetTransform());
				parentSet = true;
			}
		}
		void OnYPressed() {
			if (!parentSet && actor0 != nullptr && actor1 != nullptr) {
				actor0->GetTransform()->SetParent(actor1->GetTransform(), false);
				parentSet = true;
			}
		}
	}

	TestLevel::TestLevel(Engine& engine) : Level("TestLevel", engine) {
		Logger::Log(*this, "Running Level", Logger::Severity::Debug);

		auto actor0 = NewActor("Actor0-RectangleRenderer");
		actor0->GetTransform()->localPosition = Vector2(300, 200);
		auto& rc0 = actor0->AddComponent<RenderComponent>();
		rc0.SetRectangleRenderer(RenderFactory::NewRectangle(Vector2(300, 100), Colour::Magenta));

		auto player = NewActor("Player");
		player->GetTransform()->localPosition = Vector2(-200, -300);
		player->AddComponent<ControllerComponent>();
		auto& playerRenderer = player->AddComponent<RenderComponent>();
		playerRenderer.SetSpriteRenderer("Assets/Ship.png");
		//actor0->GetTransform()->SetParent(pawn0->GetTransform());

		//pawn0 = actor0;		// Must not compile
		auto actor1 = NewActor("Actor1-TextRenderer");
		Fixed yPos = this->engine.GetWorld().GetWorldSize().y / 2;
		auto& rc = actor1->AddComponent<RenderComponent>();
		auto& tr = rc.SetTextRenderer("Hello World!");
		tr.SetColour(Colour(200, 150, 50)).SetSize(50);
		yPos -= tr.GetBounds().y;
		actor1->GetTransform()->localPosition = Vector2(0, yPos - 50);

		// Tests
		_TestLevel::actor0 = actor0.get();
		_TestLevel::actor1 = player.get();
		_TestLevel::token0 = GetInputHandler().Register(GameInput::X, &_TestLevel::OnXPressed, OnKey::Released);
		_TestLevel::token1 = GetInputHandler().Register(GameInput::Y, &_TestLevel::OnYPressed, OnKey::Released, true);
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
