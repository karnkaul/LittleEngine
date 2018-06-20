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
		Fixed yPos = this->engine->GetWorld().GetWorldSize().y / 2;
		auto& rc = actor1->AddComponent<RenderComponent>();
		auto& tr = rc.SetTextRenderer("Hello World!");
		tr.SetColour(Colour(200, 150, 50)).SetSize(50);
		yPos -= tr.GetBounds().y;
		actor1->GetTransform()->localPosition = Vector2(0, yPos - 50);
	}

	bool parentSet = false;
	void RenderTests(Level* level, std::vector<std::shared_ptr<Actor> >& actors, RenderParams& params) {
		if (!actors.empty()) {
			// TODO: TextShape
			actors[0]->GetTransform()->Rotate(2);
			KeyState space = level->GetInput().GetKeyState(KeyCode::Space);
			KeyState control = level->GetInput().GetKeyState(KeyCode::Control);
			if (!parentSet && space.pressed) {
				actors[0]->GetTransform()->SetParent(actors[1]->GetTransform(), !control.pressed);
				parentSet = true;
			}
		}
	}

	void TestLevel::Render(RenderParams & params) {
		RenderTests(this, actors, params);
 		Level::Render(params);
	}
}
