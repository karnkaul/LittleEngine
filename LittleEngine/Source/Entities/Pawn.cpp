#include "stdafx.h"
#include "Pawn.h"
#include "PawnController.h"
#include "Engine/Logger/Logger.h"
#include "Engine/World.h"
#include "Levels/Level.h"
#include "Levels/LevelManager.h"
#include "Components/RenderComponent.h"
#include "SFMLInterface/Rendering/Renderer.h"
#include "SFMLInterface/Rendering/RenderFactory.h"
#include "SFMLInterface/Rendering/ShapeRenderer.h"
#include "SFMLInterface/InputHandler.h"

bool test = false, test2 = false, test3 = false;
std::shared_ptr<Game::Actor> testPtr;

namespace Game {
	Pawn::Pawn(Level& level, std::string name) : Actor(level, name) {
		controller = nullptr;
	}

	Pawn::~Pawn() {
		// DEBUG
		testPtr = nullptr;
		test = test2 = test3 = false;
	}

	void Pawn::FixedTick() {}

	void ClampPosition(Vector2& position, const Vector2& worldBoundsX, const Vector2& worldBoundsY, const Vector2& padding) {
		if ((position.x - padding.x) < worldBoundsX.x) {
			position.x = worldBoundsX.x + padding.x;
		}
		if ((position.x + padding.x) > worldBoundsX.y) {
			position.x = worldBoundsX.y - padding.x;
		}
		if ((position.y - padding.y) < worldBoundsY.x) {
			position.y = worldBoundsY.x + padding.y;
		}
		if ((position.y + padding.y) > worldBoundsY.y) {
			position.y = worldBoundsY.y - padding.y;
		}
	}

	void Pawn::Tick(Fixed deltaTime) {
		if (controller != nullptr) {
			controller->Tick(deltaTime, level->GetInputHandler());
		}
		if (renderComponent == nullptr) {
			renderComponent = GetComponent<RenderComponent>();
		}

		Vector2 worldX = level->GetWorld().GetWorldBoundsX();
		Vector2 worldY = level->GetWorld().GetWorldBoundsY();
		//Vector2 padding = renderer->GetWorldBounds(level->GetWorld()) * Fixed(1, 2);
		Vector2 padding = renderComponent->GetRenderer()->GetWorldBounds(level->GetWorld()) * Fixed(1, 2);
		ClampPosition(transform->localPosition, worldX, worldY, padding);

		if (level->LevelTimeMilliSeconds() > 3000 && !test) {
			Logger::Log(*this, "Construct test");
			testPtr = level->NewActor("DynamicTest");
			auto& rc = testPtr->AddComponent<RenderComponent>();
			rc.SetCircleRenderer(RenderFactory::NewCircle(50));
			testPtr->GetTransform()->localPosition = Vector2(-200, 100);
			testPtr->GetTransform()->SetParent(transform, false);
			test = true;
		}

		//if (engine->GetCurrentTime() > 4000 && !test3) {
		//	engine->DestroyActor(This());
		//	test3 = true;
		//}

		if (level->LevelTimeMilliSeconds() > 5000 && !test2) {
			level->DestroyActor(testPtr);
			testPtr = nullptr;
			test2 = true;
		}
	}

	void Pawn::AddPlayerController() {
		controller = std::make_unique<PlayerController>(*this);
	}
}
